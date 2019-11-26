#include <stddef.h>
#include <unistd.h>
#include "common.h"
#include "display.h"

void on_child(int i) {
	display_on_child(i, "");
	execl("/bin/date", "date", "+[execl /bin/date]: %A, %e %B %Y %T.%N", NULL);
}

int main(void) {
	display_header();
	display_on_parent("");

	fork_children(on_child, DEFAULT_CHILDREN_COUNT);
	wait_children(NULL, DEFAULT_CHILDREN_COUNT);
}
