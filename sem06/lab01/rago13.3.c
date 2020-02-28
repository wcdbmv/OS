#include "apue.h"
#include <pthread.h>
#include <syslog.h>

sigset_t mask;

extern int already_running(void);

void
reread(void)
{
	/* ... */
}

void *
thr_fn(void *arg)
{
	int err, signo;

	for (;;) {
		err = sigwait(&mask, &signo);
		if (err != 0) {
			syslog(LOG_ERR, "ошибка вызова функции sigwait");
			exit(1);
		}
		switch (signo) {
		case SIGHUP:
			syslog(LOG_INFO, "Чтение конфигурационного файла");
			reread();
			break;
		case SIGTERM:
			syslog(LOG_INFO, "получен сигнал SIGTERM; выход");
			exit(0);
		default:
			syslog(LOG_INFO, "получен непредвиденный сигнал %d\n", signo);
		}
	}
	return(0);
}

int
main(int argc, char *argv[])
{
	int err;
	pthread_t tid;
	char *cmd;
	struct sigaction sa;

	if ((cmd = strchr(argv[0], '/')) == NULL)
		cmd = argv[0];
	else
		cmd++;

	/*
	 * Перейти в режим демона.
	 */
	daemonize(cmd);

	/*
	 * Убедиться в том, что ранее не была запущена другая копия демона.
	 */
	if (already_running()) {
		syslog(LOG_ERR, "демон уже запущен");
		exit(1);
	}

	/*
	 * Восстановить действие по умолчанию для сигнала SIGHUP
	 * и заблокировать все сигналы.
	 */
	sa.sa_handler = SIG_DFL;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGHUP, &sa, NULL) < 0)
		err_quit("%s: невозможно восстановить действие SIG_DFL для SIGHUP");
	sigfillset(&mask);
	if ((err = pthread_sigmask(SIG_BLOCK, &mask, NULL)) != 0)
		err_exit(err, "ошибка выполнения операции SIG_BLOCK");

	/*
	 * Создать поток, который будет заниматься обработкой SIGHUP и SIGTERM.
	 */
	err = pthread_create(&tid, NULL, thr_fn, 0);
	if (err != 0)
		err_exit(err, "невозможно создать поток");

	/*
	 * Остальная часть программы-демона.
	 */
	/* ... */
	exit(0);
}
