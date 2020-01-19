#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "children.h"
#include "display.h"
#include "message.h"

int pipefd[2];

void on_child(int i) {
	display_on_child(i, "");
	send_message_to_parent(i, pipefd);
}

int main(void) {
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
