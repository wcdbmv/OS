#include <errno.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE 4096

FILE *open_proc_file(int pid, const char *filename)
{
	char pathname[PATH_MAX];
	snprintf(pathname, sizeof pathname, "/proc/%d/%s", pid, filename);

	FILE *file = fopen(pathname, "r");
	if (file == NULL) {
		fprintf(stderr, "fopen(%s, \"r\"): %s\n", pathname, strerror(errno));
		exit(EXIT_FAILURE);
	}

	return file;
}

void close_proc_file(FILE *file)
{
	fclose(file);
}

void print_proc_environ(FILE *file, int pid)
{
	char buf[BUF_SIZE];
	size_t len;

	printf("=== Environment list of the process with id %d:\n", pid);
	while ((len = fread(buf, 1, BUF_SIZE, file)) > 0) {
		for (char *c = buf; c < buf + len; ++c) {
			if (*c == '\0') {
				*c = '\n';
			}
		}
		buf[len - 1] = '\0';
		printf("%s", buf);
	}
}

void print_proc_state(FILE *file)
{
	int pid;
	char comm[NAME_MAX];
	char state;

	fscanf(file, "%d %s %c", &pid, comm, &state);

	const char *state_description = NULL;
	switch (state) {
	case 'R': state_description = "Running"; break;
	case 'S': state_description = "Sleeping (interruptible sleep — waiting an event for complete)"; break;
	case 'D': state_description = "Disk sleep (uninterruptible sleep)"; break;
	case 'T': state_description = "Stopped (stopped by job control signal)"; break;
	case 't': state_description = "Tracing stop (stopped by debugger during the tracing)"; break;
	case 'X': state_description = "Dead (should never be seen)"; break;
	case 'Z': state_description = "Zombie (terminated but not reaped by its parent)"; break;
	case 'I': state_description = "Idle (kernel thread)"; break;
	default:  state_description = "unknown"; break;
	}

	printf("\n\n=== State of the process with id %d:\n", pid);
	printf("\t%c\t%s\n", state, state_description);
}

int main(int argc, char *argv[])
{
	if (argc > 2) {
		fprintf(stderr, "Usage: %s [pid=self]\n", argv[0]);
		return EXIT_FAILURE;
	}

	const int pid = argc == 1 ? getpid() : atoi(argv[1]);

	FILE *file = NULL;

	file = open_proc_file(pid, "environ");
	print_proc_environ(file, pid);
	close_proc_file(file);

	file = open_proc_file(pid, "stat");
	print_proc_state(file);
	close_proc_file(file);
}
