#include "apue.h"
#include <syslog.h>
#include <fcntl.h>
#include <sys/resource.h>

void
daemonize(const char *cmd)
{
	int i, fd0, fd1, fd2;
	pid_t pid;
	struct rlimit rl;
	struct sigaction sa;

	/*
	 * Сбросить маску режима создания файла.
	 */
	umask(0);

	/*
	 * Получить максимально возможный номер дескриптора файла.
	 */
	if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
		err_quit("%s: невозможно получить максимальный номер дескриптора ",
		         cmd);

	/*
	 * Стать лидером новой сессии, чтобы утратить управляющий терминал.
	 */
	if ((pid = fork()) < 0)
		err_quit("%s: ошибка вызова функции fork", cmd);
	else if (pid != 0) /* родительский процесс */
		exit(0);
	setsid();

	/*
	 * Обеспечить невозможность обретения управляющего терминала в будущем.
	 */
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGHUP, &sa, NULL) < 0)
		err_quit("%s: невозможно игнорировать сигнал SIGHUP", cmd);
	if ((pid = fork()) < 0)
		err_quit("%s: ошибка вызова функции fork", cmd);
	else if (pid != 0) /* родительский процесс */
		exit(0);

	/*
	 * Назначить корневой каталог текущим рабочим каталогом,
	 * чтобы впоследствии можно было отмонтировать файловую систему.
	 */
	if (chdir("/") < 0)
		err_quit("%s: невозможно сделать текущим рабочим каталогом /",
		         cmd);

	/*
	 * Закрыть все открытые файловые дескрипторы.
	 */
	if (rl.rlim_max == RLIM_INFINITY)
		rl.rlim_max = 1024;
	for (i = 0; i < rl.rlim_max; i++)
		close(i);

	/*
	 * Присоединить файловые дескрипторы 0, 1 и 2 к /dev/null.
	 */
	fd0 = open("/dev/null", O_RDWR);
	fd1 = dup(0);
	fd2 = dup(0);

	/*
	 * Инициализировать файл журнала.
	 */
	openlog(cmd, LOG_CONS, LOG_DAEMON);
	if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
		syslog(LOG_ERR, "ошибочные файловые дескрипторы %d %d %d",
		       fd0, fd1, fd2);
		exit(1);
	}
}
