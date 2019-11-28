#include "children.h"
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
 * Дожидается завершения `children_count` потомков.
 */
void wait_children(int children_count) {
	for (int i = 0; i < children_count; ++i) {
		int status;
		const pid_t childpid = wait(&status);
		if (childpid == -1) {
			perror("wait");
			exit(EXIT_FAILURE);
		}

		if (WIFEXITED(status)) {
			display_on_parent("child %d returns %d",
			childpid, WEXITSTATUS(status));
		} else if (WIFSIGNALED(status)) {
			display_on_parent("child %d terminated with signal %d",
			childpid, WTERMSIG(status));
		} else if (WIFSTOPPED(status)) {
			display_on_parent("child%02d stopped due signal %d",
			childpid, WSTOPSIG(status));
		}
	}
}
