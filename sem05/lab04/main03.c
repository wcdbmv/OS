#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "children.h"
#include "display.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"

void on_child(int i) {
	switch (i) {
	case 0:
		execl("/bin/date", "date", "+[execl /bin/date]: %A, %e %B %Y %T.%N", NULL);
	case 1:
		execl("/bin/echo", "echo", "hello", NULL);
	default:
		execl("/bin/false", "false", NULL);
	}
}

#pragma GCC diagnostic pop

int main(void) {
	display_header();
	display_on_parent("");

	fork_children(on_child, DEFAULT_CHILDREN_COUNT);
	wait_children(DEFAULT_CHILDREN_COUNT);
}
