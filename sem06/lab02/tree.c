#include "tree.h"
#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include "color.h"
#include "vector.h"

void print_dirname(const char *indent, const char *branch, const char *name)
{
	printf("%s%s%s%s%s\n", indent, branch, COLOR_BOLDBLUE, name, COLOR_RESET);
}

void print_filename(const char *indent, const char *branch, const char *name)
{
	printf("%s%s%s\n", indent, branch, name);
}

void print_executable(const char *indent, const char *branch, const char *name)
{
	printf("%s%s%s%s%s\n", indent, branch, COLOR_BOLDGREEN, name, COLOR_RESET);
}

void print_link(const char *indent, const char *branch, const char *name)
{
	printf("%s%s%s%s%s\n", indent, branch, COLOR_BOLDCYAN, name, COLOR_RESET);
}

int tree_process(const char *dirname, const char *indent)
{
	assert(dirname && indent);

	// Запомним абсолютный путь директории, из которой вызвали
	char previous_path[PATH_MAX];
	if (getcwd(previous_path, PATH_MAX) == NULL) {
		fprintf(stderr, "[1] getcwd(%s, %d): %s\n", previous_path, PATH_MAX, strerror(errno));
		return EXIT_FAILURE;
	}

	// Перейдём в директорию, которую запросили
	if (chdir(dirname) == -1) {
		fprintf(stderr, "[2] chdir(%s): %s\n", dirname, strerror(errno));
		return EXIT_FAILURE;
	}

	// Запомним абсолютный путь этой директории
	char path[PATH_MAX];
	if (getcwd(path, PATH_MAX) == NULL) {
		fprintf(stderr, "[3] getcwd(%s, %d): %s\n", path, PATH_MAX, strerror(errno));
		chdir(previous_path);
		return EXIT_FAILURE;
	}

	// Откроем директорию по её абсолютному пути.
	// Если использовать относительный путь dirname, то может возникнуть ошибка при ".."
	DIR *dir;
	if ((dir = opendir(path)) == NULL) {
		fprintf(stderr, "[4] opendir(%s): %s\n", path, strerror(errno));
		chdir(previous_path);
		return EXIT_FAILURE;
	}

	vector_t *entries = new_vector();
	if (!entries) {
		fprintf(stderr, "[5] new_vector: %s\n", strerror(errno));
		chdir(previous_path);
		return EXIT_FAILURE;
	}

	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL) {
		if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
			if (push_vector(entries, entry) != EXIT_SUCCESS) {
				fprintf(stderr, "[6] push_vector: %s\n", strerror(errno));
				delete_vector(&entries);
				closedir(dir);
				chdir(previous_path);
				return EXIT_FAILURE;
			}
		}
	}

	const char *current_branch    = NULL;
	const char *additional_indent = NULL;
	struct stat st;
	for (size_t i = 0; i < entries->size; ++i) {
		entry = entries->data[i];
		if (i < entries->size - 1) {
			current_branch    = "├── ";
			additional_indent = "│   ";
		} else {
			current_branch    = "└── ";
			additional_indent = "    ";
		}

		char next_indent[PATH_MAX];
		strncpy(next_indent, indent, PATH_MAX - 1);
		next_indent[PATH_MAX - 1] = '\0';
		strcat(next_indent, additional_indent);

		if (entry->d_type == DT_DIR) {
			print_dirname(indent, current_branch, entry->d_name);
			tree_process(entry->d_name, next_indent);
		} else {
			if (lstat(entry->d_name, &st) == -1) {
				fprintf(stderr, "[7] lstat(%s, &st): %s\n", path, strerror(errno));
				delete_vector(&entries);
				closedir(dir);
				chdir(previous_path);
				return EXIT_FAILURE;
			}

			if ((st.st_mode & S_IFMT) == S_IFLNK) {
				print_link(indent, current_branch, entry->d_name);
			} else if (st.st_mode & S_IXUSR) {
				print_executable(indent, current_branch, entry->d_name);
			} else {
				print_filename(indent, current_branch, entry->d_name);
			}
		}
	}

	delete_vector(&entries);

	if (closedir(dir) == -1) {
		fprintf(stderr, "[8] closedir: %s\n", strerror(errno));
		chdir(previous_path);
		return EXIT_FAILURE;
	}

	if (chdir(previous_path) == -1) {
		fprintf(stderr, "[9] chdir(%s): %s\n", previous_path, strerror(errno));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int tree(const char *root) {
	assert(root);

	print_dirname("", "", root);
	return tree_process(root, "");
}
