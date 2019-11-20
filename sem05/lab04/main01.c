#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void display_header(void) {
	printf("\n PPID   PID  PGID description\n");
}

void display_row(const char *description) {
	printf("%5d %5d %5d %s\n", getppid(), getpid(), getpgrp(), description);
}

void on_parent(void) {
	display_row("parent");
}

void on_child(void) {
	display_row("child before sleep");
	sleep(1);
	display_row("child after sleep");
}

int main(void) {
	display_header();

	const pid_t childpid = fork();
	if (childpid == -1) {
		perror("fork");
		return EXIT_FAILURE;
	}

	childpid ? on_parent() : on_child();
}
