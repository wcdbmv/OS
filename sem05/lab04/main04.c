#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "common.h"
#include "display.h"
#include "message.h"

int pipefd[2];

void on_child(int i) {
	display_row_formatted("on child%02d", i);
	receive_message_on_child(i, pipefd);
}

void prewait(int i) {
	send_message_to_child(i, pipefd);
}

int main(void) {
	if (pipe(pipefd) == -1) {
		perror("pipe");
		return EXIT_FAILURE;
	}

	display_header();
	display_row("on parent");

	fork_children(on_child, DEFAULT_CHILDREN_COUNT);
	wait_children(prewait, DEFAULT_CHILDREN_COUNT);
}
