#include "display.h"
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>

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

