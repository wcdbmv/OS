#include <stdio.h>
#include <unistd.h>

int main() {
	const pid_t childpid = fork();
	if (!~childpid) {
		perror("Can't fork.");
		return 1;
	}

	const char *format = childpid ? " %d " : "%d ";

	for (;;) {
		printf(format, getpid());
	}
}
