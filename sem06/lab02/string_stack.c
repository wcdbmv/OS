#include "string_stack.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int push_string(stack_node_t **stack_node, const char *string)
{
	assert(stack_node && string);

	const size_t size = strlen(string) + 1;
	return push_stack(stack_node, string, size);
}

char *pop_string(stack_node_t **stack_node)
{
	assert(stack_node);

	return pop_stack(stack_node);
}

const char *top_string(const stack_node_t *stack_node)
{
	assert(stack_node);

	return top_string(stack_node);
}

void delete_string(char **string)
{
	assert(string);

	free(*string);
	*string = NULL;
}
