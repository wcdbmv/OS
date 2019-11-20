#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include "display.h"

#define CHILDREN_COUNT 2

void on_child(int i) {
	display_row_formatted("on child%02d", i);
	execl("/bin/date", "date", "+[execl /bin/date]: %A, %e %B %Y %T.%N", NULL);
}

int main(void) {
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
