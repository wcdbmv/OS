#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include "display.h"

#define CHILDREN_COUNT 3
#define BUFFER_SIZE 32
#define SLEEP_TIME 3

int pipefd[2];
bool access_flag = false;

void sighandler(__attribute__((unused)) int signum) {
	access_flag = true;
}

void on_child(int i) {
	display_row_formatted("on child%02d", i);
	sleep(SLEEP_TIME);

	// get message
	if (access_flag) {
		close(pipefd[1]);
		char buffer[BUFFER_SIZE];
		read(pipefd[0], buffer, sizeof buffer);
		display_row_formatted("on child%02d [receives \"%s\"]", i, buffer);
	}
}

void send_message_to_child(int i) {
	close(pipefd[0]);
	char message[] = "message to childxx";
	snprintf(message, sizeof message, "message to child%02d", i);
	write(pipefd[1], message, sizeof message);
}

int main(void) {
	if (signal(SIGINT, sighandler) == SIG_ERR) {
		perror("signal");
		return EXIT_FAILURE;
	}

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
		send_message_to_child(i);

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
