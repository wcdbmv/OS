#include "display.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define STRING_LIMIT 64

void display_header(void) {
	printf(" PPID   PID  PGID description\n");
}

void display_row(const char *description) {
	printf("%5d %5d %5d %s\n", getppid(), getpid(), getpgrp(), description);
}

void display_row_f(const char *format, ...) {
	char description[STRING_LIMIT];

	va_list ap;
	va_start(ap, format);
	vsnprintf(description, sizeof description, format, ap);
	va_end(ap);

	display_row(description);
}

void display_row_fv(const char *format, va_list ap) {
	char description[STRING_LIMIT];
	vsnprintf(description, sizeof description, format, ap);
	display_row(description);
}

void display_on_parent(const char *format, ...) {
	char new_format[STRING_LIMIT];
	snprintf(new_format, sizeof new_format, "[%s] %s", "on  parent", format);

	va_list ap;
	va_start(ap, format);
	display_row_fv(new_format, ap);
	va_end(ap);
}

void display_on_child(int i, const char *format, ...) {
	char new_format[STRING_LIMIT];
	snprintf(new_format, sizeof new_format, "[%s%02d] %s", "on child", i, format);

	va_list ap;
	va_start(ap, format);
	display_row_fv(new_format, ap);
}
