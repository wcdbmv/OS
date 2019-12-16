#include "children.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"

int fork_children(int children_count, void (*on_child)(int id)) {
	int forks = 0;
	for (int i = 0; i < children_count; ++i) {
		switch (fork()) {
		case -1:
			perror("fork");
			exit(EXIT_FAILURE);
		case 0:
			if (on_child) on_child(i);
			exit(EXIT_SUCCESS);
		default:
			++forks;
		}
	}

	return forks;
}

#pragma GCC diagnostic push

void wait_children(int children_count) {
	while (children_count--) {
		int status;
		const pid_t childpid = wait(&status);
		if (childpid == -1) {
			perror("wait");
			exit(EXIT_FAILURE);
		}

		if (WIFEXITED(status)) {
			printf("[on  parent] child %d returns %d",
				childpid, WEXITSTATUS(status));
		} else if (WIFSIGNALED(status)) {
			printf("[on  parent] child %d terminated with signal %d",
				childpid, WTERMSIG(status));
		} else if (WIFSTOPPED(status)) {
			printf("[on  parent] child %d stopped due signal %d",
				childpid, WSTOPSIG(status));
		}
	}
}
