#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "color.h"

int main(void) {
	const pid_t childpid = fork();
	if (childpid == -1) {
		perror("fork");
		return EXIT_FAILURE;
	}

	const char *format = childpid ? COLOR_RED("%d ") : COLOR_CYAN("%d ");

	for (;;) {
		printf(format, getpid());
	}
}
