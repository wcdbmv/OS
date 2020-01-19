#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "children.h"
#include "display.h"
#include "message.h"

#define SLEEP_TIME 3

int pipefd[2];
bool access_flag = false;

void sighandler(__attribute__((unused)) int signum) {
	access_flag = true;
}

void on_child(int i) {
	display_on_child(i, "");
	sleep(SLEEP_TIME);

	if (access_flag) {
		send_message_to_parent(i, pipefd);
	}
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
	display_on_parent("");

	fork_children(on_child, DEFAULT_CHILDREN_COUNT);
	wait_children(DEFAULT_CHILDREN_COUNT);

	receive_message_on_parent(pipefd);
}
