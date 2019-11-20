#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include "display.h"
#include "message.h"

#define CHILDREN_COUNT 3

int pipefd[2];

void on_child(int i) {
	display_row_formatted("on child%02d", i);
	receive_message_on_child(i, pipefd);
}

int main(void) {
	if (pipe(pipefd) == -1) {
		perror("pipe");
		return EXIT_FAILURE;
	}

	display_header();
	display_row("on parent");

	for (int i = 0; i < CHILDREN_COUNT; ++i) {
		switch (fork()) {
		case -1:
			perror("fork");
			return EXIT_FAILURE;
		case 0:
			on_child(i);
			return EXIT_SUCCESS;
		}
	}

	for (int i = 0; i < CHILDREN_COUNT; ++i) {
		send_message_to_child(i, pipefd);

		int status;
		const pid_t childpid = wait(&status);
		if (childpid == -1) {
			perror("wait");
			return EXIT_FAILURE;
		}

		if (WIFEXITED(status)) {
			display_row_formatted("on parent [child%02d returns %d]", i, WEXITSTATUS(status));
		} else {
			display_row_formatted("on parent [child%02d terminated abnormally]", i);
		}
	}
}
