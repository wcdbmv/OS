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
#include <unistd.h>
#include "error.h"

#define SLEEP_TIME 5

void syslog_quit(const char *prompt) {
	syslog(LOG_ERR, "Unable to %s: %s", prompt, strerror(errno));
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

int lockfile(int fd) {
	struct flock fl = {
		.l_type = F_WRLCK,
		.l_start = 0,
		.l_whence = SEEK_SET,
		.l_len = 0
	};
	return fcntl(fd, F_SETLK, &fl);
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

void deny_allocate_controlling_ttys() {
	struct sigaction sa;
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGHUP, &sa, NULL) == -1) {
		syslog_quit("ignore SIGHUP");
	}
}

void daemonize(const char *cmd) {
	// Инициализировать файл журнала
	openlog(cmd, LOG_CONS, LOG_DAEMON);

	// 1. Сбросить маску режима создания файлов
	umask(0);

	// 2. Вызвать функцию fork и завершить родительский процесс. Этим самым
	// мы гарантируем, что дочерний процесс не будет являться лидером
	// группы, а это необходимое условие для вызова функции setsid
	const pid_t pid = fork();
	if (pid == -1) {
		syslog_quit("fork");
	} else if (pid != 0) {
		exit(EXIT_SUCCESS);
	}

	// 3. Создать новую сессию, при этом процесс становится (а) лидером
	// новой сессии (б) лидером новой группы процессов и (в) лишается
	// управляющего терминала
	setsid();

	// Обеспечить невозможность обретения управляющего терминала в будущем
	deny_allocate_controlling_ttys();

	// 4. Сделать корневой каталог текущим рабочим каталогом
	if (chdir("/") == -1) {
		syslog_quit("chdir");
	}

	// 5. Закрыть все открытые файловые дескрипторы
	for (int fd = sysconf(_SC_OPEN_MAX); fd >= 0; --fd) {
                close(fd);
        }

	// Присоединить файловые дескрипторы 0, 1 и 2 к /dev/null
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

	syslog(LOG_WARNING, "STARTS");

	for (int runtime = 0;; runtime += SLEEP_TIME) {
		syslog(LOG_INFO, "RUNNING %d SEC", runtime);
		sleep(SLEEP_TIME);
	}
}
