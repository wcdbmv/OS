#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <unistd.h>
#include "error.h"

#define LOCKFILE "/var/run/daemon.pid"
#define LOCKMODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

int lockfile(int fd) {
    struct flock fl;
    fl.l_type = F_WRLCK;
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;
    return fcntl(fd, F_SETLK, &fl);
}

int already_running(void) {
	char buf[16];

	const int fd = open(LOCKFILE, O_RDWR|O_CREAT, LOCKMODE);
	if (fd == -1) {
		syslog(LOG_ERR, "не возможно открыть %s: %s",
		       LOCKFILE, strerror(errno));
		exit(EXIT_FAILURE);
	}
	if (lockfile(fd) == -1) {
		if (errno == EACCES || errno == EAGAIN) {
			close(fd);
			return EXIT_FAILURE;
		}
		syslog(LOG_ERR, "невозможно установить блокировку на %s: %s",
		       LOCKFILE, strerror(errno));
		exit(EXIT_FAILURE);
	}
	ftruncate(fd, 0);
	sprintf(buf, "%ld", (long) getpid());
	write(fd, buf, strlen(buf) + 1);
	return EXIT_SUCCESS;
}

void daemonize(const char *cmd) {
	// Сбросить маску режима создания файла
	umask(0);

	// Получить максимально возможный номер дескриптора файла
	struct rlimit rl;
	if (getrlimit(RLIMIT_NOFILE, &rl) == -1) {
		err_quit("%s: невозможно получить максимальный номер дескриптора", cmd);
	}

	// Стать лидером новой сессии, чтобы утратить управляющий терминал
	switch (fork()) {
	case -1:
		err_quit("%s: ошибка вызова функции fork", cmd);
	case 0:
		exit(EXIT_SUCCESS);
	}
	setsid();

	// Обеспечить невозможность обретения управляющего терминала в будущем
	struct sigaction sa;
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGHUP, &sa, NULL) == -1) {
		err_quit("%s: невозможно игнорировать сигнал SIGHUP", cmd);
	}

	// Назначить корневой каталог текущим рабочим каталогом,
	// чтобы впоследствии можно было отмонтировать файловую систему
	if (chdir("/") == -1) {
		err_quit("%s: невозможно сделать текущим рабочим каталогом /", cmd);
	}

	// Закрыть все открытые файловые дескрипторы
	if (rl.rlim_max == RLIM_INFINITY) {
		rl.rlim_max = 1024;
	}
	for (int i = 0; i < rl.rlim_max; ++i) {
		close(i);
	}

	// Присоединить файловые дескрипторы 0, 1 и 2 к /dev/null
	const int fd0 = open("/dev/null", O_RDWR);
	const int fd1 = dup(0);
	const int fd2 = dup(0);

	// Инициализировать файл журнала
	openlog(cmd, LOG_CONS, LOG_DAEMON);
	if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
		syslog(LOG_ERR, "ошибочные файловые дескрипторы %d %d %d", fd0, fd1, fd2);
		exit(EXIT_FAILURE);
	}
}

int main(void) {
	daemonize("--my-daemon--");

	if (already_running() != EXIT_SUCCESS) {
		syslog(LOG_ERR, "--my-daemon-- already running");
		exit(EXIT_FAILURE);
	}

	syslog(LOG_WARNING, "--my-daemon-- starts");

	for (;;) {
		syslog(LOG_INFO, "--my-daemon-- running");
		sleep(5);
	}
}
