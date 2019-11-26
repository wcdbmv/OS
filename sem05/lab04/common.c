#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include "display.h"

/*
 * Создаёт `children_count` потомков, каждый из которых исполняет функцию
 * `on_child` (если передана) и завершается.
 */
void fork_children(void (*on_child)(int i), int children_count) {
	for (int i = 0; i < children_count; ++i) {
		switch (fork()) {
		case -1:
			perror("fork");
			exit(EXIT_FAILURE);
		case 0:
			if (on_child) on_child(i);
			exit(EXIT_SUCCESS);
		}
	}
}

/*
 * Дожидается завершения `children_count` потомков. Перед завершением i-го
 * потомка исполняет функцию `prewait` (если передана).
 */
void wait_children(void (*prewait)(int i), int children_count) {
	for (int i = 0; i < children_count; ++i) {
		if (prewait) prewait(i);

		int status;
		const pid_t childpid = wait(&status);
		if (childpid == -1) {
			perror("wait");
			exit(EXIT_FAILURE);
		}

		if (WIFEXITED(status)) {
			display_row_formatted("on parent [child%02d returns %d]", i, WEXITSTATUS(status));
		} else {
			display_row_formatted("on parent [child%02d terminated abnormally]", i);
		}
	}
}
