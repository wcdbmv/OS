#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "color.h"

#define COLOR_REG  COLOR_FG_DEFAULT
#define COLOR_DIR  COLOR_BOLD COLOR_FG_BLUE
#define COLOR_BLK  COLOR_BOLD COLOR_FG_YELLOW COLOR_BG_DARKGRAY
#define COLOR_CHR  COLOR_BOLD COLOR_FG_YELLOW COLOR_BG_DARKGRAY
#define COLOR_FIFO COLOR_FG_YELLOW COLOR_BG_DARKGRAY
#define COLOR_LINK COLOR_BOLD COLOR_FG_CYAN
#define COLOR_SOCK COLOR_BOLD COLOR_FG_MAGENTA
#define COLOR_EXE  COLOR_BOLD COLOR_FG_GREEN

#define FTW_F   1  // файл, не являющийся каталогом
#define FTW_D   2  // каталог
#define FTW_DNR 3  // каталог, недоступный для чтения
#define FTW_NS  4  // файл, информацию о котором нельзя получить с помощью stat

// тип функции, которая будет вызываться для каждого встреченного файла
typedef int MyFunc(const char *filename, const struct stat *st, int depth, int type);

static MyFunc counter;
static int myftw(const char *, MyFunc *);
static int dopath(const char *filename, int depth, MyFunc *);

static size_t nreg, ndir, nblk, nchr, nfifo, nlink, nsock, ntot;

int main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <dir>\n", argv[0]);
		return EXIT_FAILURE;
	}

	const int ret = myftw(argv[1], counter);

	ntot = nreg + ndir + nblk + nchr + nfifo + nlink + nsock;
	if (ntot == 0) {
		ntot = 1; // во избежание деления на 0; вывести 0 для всех счётчиков
	}

	printf("\n─────────── Summary ───────────\n");
	printf("regular files: %7ld, %5.2f %%\n", nreg,  nreg  * 100.0 / ntot);
	printf("directories:   %7ld, %5.2f %%\n", ndir,  ndir  * 100.0 / ntot);
	printf("block devices: %7ld, %5.2f %%\n", nblk,  nblk  * 100.0 / ntot);
	printf("char devices:  %7ld, %5.2f %%\n", nchr,  nchr  * 100.0 / ntot);
	printf("FIFOs:         %7ld, %5.2f %%\n", nfifo, nfifo * 100.0 / ntot);
	printf("symbolic links:%7ld, %5.2f %%\n", nlink, nlink * 100.0 / ntot);
	printf("sockets:       %7ld, %5.2f %%\n", nsock, nsock * 100.0 / ntot);
	printf("Total:         %7ld\n", ntot);

	return ret;
}

// Обходит дерево каталогов, начиная с каталога pathname, применяя к каждому файлу функцию func.
static int myftw(const char *pathname, MyFunc *func)
{
	return dopath(pathname, 0, func);
}

static int dopath(const char *filename, int depth, MyFunc *func)
{
	struct stat statbuf;
	struct dirent *entry;
	DIR *dp;
	int ret;

	if (lstat(filename, &statbuf) == -1) {
		return func(filename, &statbuf, depth, FTW_NS);
	}

	if (S_ISDIR(statbuf.st_mode) == 0) {  // не каталог
		return func(filename, &statbuf, depth, FTW_F);
	}

	if ((ret = func(filename, &statbuf, depth, FTW_D)) != EXIT_SUCCESS) {
		return ret;
	}

	if ((dp = opendir(filename)) == NULL) {  // каталог недоступен
		return func(filename, &statbuf, depth, FTW_DNR);
	}

	chdir(filename);
	while ((entry = readdir(dp)) != NULL) {
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
			continue;
		}

		/* рекурсивный вызов */
		if ((ret = dopath(entry->d_name, depth + 1, func)) != EXIT_SUCCESS) {
			return ret;  // выход по ошибке
		}
	}
	chdir("..");

	if (closedir(dp) == -1) {
		fprintf(stderr, "closedir(%s): %s", filename, strerror(errno));
	}

	return ret;  // выход по завершении обхода
}

static int counter(const char *filename, const struct stat *st, int depth, int type)
{
	for (int i = 0; i + 1 < depth; ++i) {
		printf("│   ");
	}

	const char *s = "";

	switch (type) {
	case FTW_F:
		switch (st->st_mode & S_IFMT) {
		case S_IFREG:  ++nreg;  s = st->st_mode & S_IXUSR ? COLOR_EXE : COLOR_REG; break;
		case S_IFBLK:  ++nblk;  s = COLOR_BLK;  break;
		case S_IFCHR:  ++nchr;  s = COLOR_CHR;  break;
		case S_IFIFO:  ++nfifo; s = COLOR_FIFO; break;
		case S_IFLNK:  ++nlink; s = COLOR_LINK; break;
		case S_IFSOCK: ++nsock; s = COLOR_SOCK; break;
		case S_IFDIR:
			fprintf(stderr, "Dir file type is not expected\n");
			return EXIT_FAILURE;
		default:
			fprintf(stderr, "Unknown file type '%s': %d\n", filename, st->st_mode & S_IFMT);
			return EXIT_FAILURE;
		}
		printf("├── %s%s%s\n", s, filename, COLOR_RESET);
		break;
	case FTW_D:
		++ndir;
		printf("%s%s%s%s\n", depth ? "├── " : "", COLOR_DIR, filename, COLOR_RESET);
		break;
	case FTW_DNR:
		fprintf(stderr, "Can't read directory %s: %s\n", filename, strerror(errno));
		return EXIT_FAILURE;
	case FTW_NS:
		switch (errno) {
		case EACCES:       s = "Permission denied";                 break;
		case EFAULT:       s = "Bad address";                       break;
		case ELOOP:        s = "Too many levels of symbolic links"; break;
		case ENAMETOOLONG: s = "File name too long";                break;
		case ENOENT:       s = "No such file or directory";         break;
		case ENOMEM:       s = "Cannot allocate memory";            break;
		case ENOTDIR:      s = "Not a directory";                   break;
		default:           s = strerror(errno);                     break;
		}
		fprintf(stderr, "lstat(%s, &st): %s\n", filename, s);
		return EXIT_FAILURE;
	default:
		fprintf(stderr, "Unknown file type: %d\n", type);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
