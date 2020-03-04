#include <stdio.h>
#include <stdlib.h>
#include "tree.h"

int main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <dir>\n", argv[0]);
		return EXIT_FAILURE;
	}

	return tree(argv[1]);
}
