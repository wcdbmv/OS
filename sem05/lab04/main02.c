#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define CHILDREN_COUNT 20
#define FORMATTED_DESCRIPTION_LIMIT 64

void display_header(void) {
	printf(" PPID   PID  PGID description\n");
}

void display_row(const char *description) {
	printf("%5d %5d %5d %s\n", getppid(), getpid(), getpgrp(), description);
}

void display_row_formatted(const char *format, ...) {
	char description[FORMATTED_DESCRIPTION_LIMIT];

	va_list ap;
	va_start(ap, format);
	vsnprintf(description, sizeof description, format, ap);
	va_end(ap);

	display_row(description);
}

void on_child(int i) {
	sleep(1);
	display_row_formatted("on child%02d", i);
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
