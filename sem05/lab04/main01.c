#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "display.h"

void on_parent(void) {
	display_row("parent");
}

void on_child(void) {
	display_row("child before sleep");
	sleep(1);
	display_row("child after sleep");
}

int main(void) {
	puts("");
	display_header();

	const pid_t childpid = fork();
	if (childpid == -1) {
		perror("fork");
		return EXIT_FAILURE;
	}

	childpid ? on_parent() : on_child();
}
