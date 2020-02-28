#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include "error.h"

#define SLEEP_TIME 5

/*
 * ps -ajx
 *
 * Демон — лидер группы и сессии, не имеет управляющего терминала.
 * Пользователь не должен влиять на него из командной строки.
 *
 * PROCESS STATE CODES:
 * D — uninterruptible sleep (usually IO)
 * I — Idle kernel thread
 * R — running or runnable (on run queue)
 * S — interruptible sleep (waiting for an event to complete)
 * T — stopped by job control signal
 * t — stopped by debugger during the tracing
 * W — paging (not valid since the 2.6.xx kernel)
 * X — dead (should never be seen)
 * Z — defunct ("zombie") process, terminated but not reaped by its parent
 *
 * Какой сон можно прервать?
 * Если процесс блокирован на каком-то событии, такой сон можно прервать.
 * (Нельзя прервать, если процесс блокирован на в/в)
 */

void syslog_quit(const char *prompt) {
	syslog(LOG_ERR, "Unable to %s: %m", prompt);
	exit(EXIT_FAILURE);
}

void fsyslog_quit(const char *format, ...) {
	char prompt[256];

	va_list ap;
	va_start(ap, format);
	vsnprintf(prompt, sizeof prompt, format, ap);
	va_end(ap);

	syslog_quit(prompt);
}

#define LOCKFILE "/var/run/daemon.pid"
#define LOCKMODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)
// S_IRUSR — владелец может читать
// S_IWUSR — владелец может писать
// S_IRGRP — группа-владелец может читать
// S_IROTH — все остальные могут читать

int lockfile(int fd) {
	struct flock fl = {
		.l_type = F_WRLCK,      // Режим блокировки (F_WRLCK — блокировка записи)
		.l_start = 0,           // Относительное смещение в байтах, зависит от l_whence
		.l_whence = SEEK_SET,   // Считать смещение от начала файла
		.l_len = 0              // Длина блокируемой области в байтах (0 — до конца файла)
	};
	return fcntl(fd, F_SETLK, &fl); // Установить блокировку
}

int already_running(void) {
	const int fd = open(LOCKFILE, O_RDWR|O_CREAT, LOCKMODE);
	if (fd == -1) {
		fsyslog_quit("open %s", LOCKFILE);
	}
	if (lockfile(fd) == -1) {
		if (errno == EACCES || errno == EAGAIN) {
			close(fd);
			return EXIT_FAILURE;
		}
		fsyslog_quit("lock %s", LOCKFILE);
	}
	char buf[16];
	ftruncate(fd, 0);
	sprintf(buf, "%ld", (long) getpid());
	write(fd, buf, strlen(buf) + 1);
	return EXIT_SUCCESS;
}

void daemonize(const char *cmd) {
	// Инициализировать файл журнала
	// (ALERT: У Раго это сделано в конце функции)
	openlog(cmd, LOG_CONS, LOG_DAEMON);

	// 1. Сбросить маску режима создания файлов. Маска наследуется и может
	// маскировать некоторые биты прав доступа.
	umask(0);

	// Получить максимально возможный номер дескриптора файла.
	// (ALERT: Ещё раз, ничего перемещать нельзя, Н. Ю. банит)
	struct rlimit rl;
	if (getrlimit(RLIMIT_NOFILE, &rl) == -1) {
		syslog_quit("getrlimit");
	}

	// 2. Вызвать функцию fork и завершить родительский процесс. Этим самым
	// мы гарантируем, что дочерний процесс не будет являться лидером
	// группы, а это необходимое условие для вызова функции setsid
	const pid_t pid = fork();
	if (pid == -1) {
		syslog_quit("fork");
	} else if (pid != 0) {
		exit(EXIT_SUCCESS);
	}

	// Обеспечить невозможность обретения управляющего терминала в будущем.
	// SIGHUP — сигнал, посылаемый процессу для уведомления о потере
	// соединения с управляющим терминалом пользователя.
	struct sigaction sa;
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGHUP, &sa, NULL) == -1) {
		syslog_quit("ignore SIGHUP");
	}

	// 3. Создать новую сессию, при этом процесс становится (а) лидером
	// новой сессии (б) лидером новой группы процессов и (в) лишается
	// управляющего терминала
	if (setsid() == -1) {
		syslog_quit("setsid");
	}

	// 4. Сделать корневой каталог текущим рабочим каталогом
	if (chdir("/") == -1) {
		syslog_quit("chdir");
	}

	// 5. Закрыть все открытые файловые дескрипторы. Это ненужные процессу-
	// демону файловые дескрипторы, закрытию которых он может препятствовать
	//
	// The value RLIM_INFINITY, defined in <sys/resource.h>, is considered
	// to be larger than any other limit value. If a call to getrlimit()
	// returns RLIM_INFINITY for a resource, it means the implementation
	// does not enforce limits on that resource. Specifying RLIM_INFINITY as
	// any resource limit value on a successful call to setrlimit() inhibits
	// enforcement of that resource limit.
	// (https://pubs.opengroup.org/onlinepubs/7908799/xsh/getrlimit.html)
	if (rl.rlim_max == RLIM_INFINITY) {
		rl.rlim_max = 1024;
	}
	for (rlim_t fd = 0; fd < rl.rlim_max; ++fd) {
		close(fd);
	}

	// 6. Присоединить файловые дескрипторы 0, 1 и 2 к /dev/null.
	//
	// Для того, чтобы можно было использовать функции стандартных библиотек
	// и они не выдавали ошибки.
	if (open("/dev/null", O_RDWR) != 0) {
		syslog_quit("open /dev/null");
	}
	(void) dup(0);
	(void) dup(1);
}

int main(void) {
	daemonize("DAEMON");

	if (already_running() != EXIT_SUCCESS) {
		syslog(LOG_ERR, "ALREADY RUNNING");
		exit(EXIT_FAILURE);
	}

	time_t t = time(NULL);
	syslog(LOG_WARNING, "STARTS %s", asctime(localtime(&t)));

	for (;;) {
		t = time(NULL);
		syslog(LOG_INFO, "current time is %s", asctime(localtime(&t)));
		sleep(SLEEP_TIME);
	}
}
