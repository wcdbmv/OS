#include <stdio.h>
#include <unistd.h>
#include "common.h"
#include "display.h"

#define SLEEP_TIME 1

void on_child(int i) {
	display_on_child(i, "before sleep");
	sleep(SLEEP_TIME);
	display_on_child(i, "after sleep");
}

int main(void) {
	// for pretty out run app01 in background (./app01 &)
	putchar('\n');
	display_header();
	display_on_parent("");

	fork_children(on_child, DEFAULT_CHILDREN_COUNT);
}
