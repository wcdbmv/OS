#include <stddef.h>
#include "common.h"
#include "display.h"

void on_child(int i) {
	display_row_formatted("on child%02d", i);
}

int main(void) {
	display_header();
	display_row("on parent");

	fork_children(on_child, DEFAULT_CHILDREN_COUNT);
	wait_children(NULL, DEFAULT_CHILDREN_COUNT);
}
