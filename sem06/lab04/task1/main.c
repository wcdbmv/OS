#include <errno.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE 4096

void open_proc_file(FILE *file, int pid, const char *filename)
{
	char pathname[PATH_MAX];
	snprintf(pathname, sizeof pathname, "/proc/%d/%s", pid, filename);

	file = fopen(pathname, "r");
	if (file == NULL) {
		fprintf(stderr, "fopen(%s, \"r\"): %s\n", pathname, strerror(errno));
		exit(EXIT_FAILURE);
	}
}

void close_proc_file(FILE *file)
{
	fclose(file);
}

void print_proc_environ(FILE *file)
{
	char buf[BUF_SIZE];
	size_t len;

	while ((len = fread(buf, 1, BUF_SIZE, file)) > 0) {
		for (char *c = buf; c < buf + len; ++c) {
			if (*c == '\0') {
				*c = '\n';
			}
		}
		buf[len - 1] = '\0';
		printf("%s", buf);
	}
	putchar('\n');
}

int main(int argc, char *argv[])
{
	if (argc > 2) {
		fprintf(stderr, "Usage: %s [pid=self]\n", argv[0]);
		return EXIT_FAILURE;
	}

	const int pid = argc == 1 ? getpid() : atoi(argv[1]);

	FILE *file = NULL;

	open_proc_file(file, pid, "environ");
	printf("Environment list of the process with id %d:\n", pid);
	print_proc_environ(file);
	close_proc_file(file);
}
