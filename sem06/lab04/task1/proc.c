#include <errno.h>
#include <dirent.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE 4096

FILE *open_proc_file(int pid, const char *filename);
void close_proc_file(FILE *file);
void print_proc_file(FILE *file, char delim);
void print_proc_environ(FILE *file, int pid);
void print_proc_cmdline(FILE *file, int pid);
void print_proc_stat(FILE *file);
void print_proc_fd(int pid);

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
	print_proc_stat(file);
	close_proc_file(file);

	file = open_proc_file(pid, "cmdline");
	print_proc_cmdline(file, pid);
	close_proc_file(file);

	print_proc_fd(pid);
}

/*
 * Открытие файла /proc/<pid>/<filename>
 */
FILE *open_proc_file(int pid, const char *filename)
{
	char pathname[PATH_MAX];
	snprintf(pathname, sizeof pathname, "/proc/%d/%s", pid, filename);

	FILE *file = fopen(pathname, "r");
	if (file == NULL) {
		fprintf(stderr, "fopen(\"%s\", \"r\"): %s\n", pathname, strerror(errno));
		exit(EXIT_FAILURE);
	}

	return file;
}

/*
 * Закрытие файла
 */
void close_proc_file(FILE *file)
{
	fclose(file);
}

/*
 * Вывод на экран файла file с заменой символа '\0' на символ delim
 */
void print_proc_file(FILE *file, char delim)
{
	char buf[BUF_SIZE];
	size_t len;

	while ((len = fread(buf, 1, BUF_SIZE, file)) > 0) {
		for (char *c = buf; c < buf + len; ++c) {
			if (*c == '\0') {
				*c = delim;
			}
		}
		buf[len - 1] = '\0';
		printf("%s", buf);
	}
}

/*
 * Вывод информации об окружении процесса
 */
void print_proc_environ(FILE *file, int pid)
{
	printf("=== Environment list of the process with id %d:\n", pid);
	print_proc_file(file, '\n');
}

/*
 * Вывод информации из файла cmdline
 */
void print_proc_cmdline(FILE *file, int pid)
{
	printf("\n=== Command line for the process with id %d:\n", pid);
	print_proc_file(file, ' ');
}

/*
 * Вывод информации о состоянии процесса
 */
void print_proc_stat(FILE *file)
{
	static char *fields[] = {
		"pid", "comm", "state", "ppid", "pgrp", "session", "tty_nr", "tpgid",
		"flags", "minflt", "cminflt", "majflt", "cmajflt", "utime", "stime",
		"cutime", "cstime", "priority", "nice", "num_threads", "itrealvalue",
		"starttime", "vsize", "rss", "rsslim", "startcode", "endcode",
		"startstack", "kstkesp", "kstkeip", "signal", "blocked", "sigignore",
		"sigcatch", "wchan", "nswap", "cnswap", "exit_signal", "processor",
		"rt_priority", "policy", "delayacct_blkio_ticks", "guest_time",
		"cguest_time", "start_data", "end_data", "start_brk", "arg_start",
		"arg_end", "env_start", "env_end", "exit_code", NULL
	};

	char buf[BUF_SIZE];
	const size_t len = fread(buf, 1, BUF_SIZE, file);
	buf[len - 1] = '\0';

	printf("\n\n=== State of the process:\n");
	for (
		char *pch = strtok(buf, " "), **pfield = fields;
		pch && *pfield;
		pch = strtok(NULL, " "), ++pfield
	) {
		printf("%-21s %s\n", *pfield, pch);
	}
}

/*
 * Вывод информации из директории fd
 */
void print_proc_fd(int pid)
{
	char dirname[PATH_MAX];
	snprintf(dirname, sizeof dirname, "/proc/%d/fd", pid);

	DIR *dir = opendir(dirname);
	if (dir == NULL) {
		fprintf(stderr, "opendir(\"%s\"): %s\n", dirname, strerror(errno));
		exit(EXIT_FAILURE);
	}

	printf("\n\n=== List of open files for the process with id %d:\n", pid);
	struct dirent *entry = NULL;
	while ((entry = readdir(dir)) != NULL) {
		if (strcmp(entry->d_name, ".") == 0
		 || strcmp(entry->d_name, "..") == 0) {
			continue;
		}

		char path[PATH_MAX], str[PATH_MAX];
		snprintf(path, sizeof path, "%s/%s", dirname, entry->d_name);

		const int n = readlink(path, str, sizeof str);
		str[n] = '\0';

		printf("\t%-17s -> %s\n", path, str);
	}

	closedir(dir);
}
