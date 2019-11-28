#include <stddef.h>
#include "children.h"
#include "display.h"

void on_child(int i) {
	display_on_child(i, "");
}

int main(void) {
	display_header();
	display_on_parent("");

	fork_children(on_child, DEFAULT_CHILDREN_COUNT);
	wait_children(DEFAULT_CHILDREN_COUNT);
}
