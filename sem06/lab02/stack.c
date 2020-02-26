#include "stack.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

const void *top_stack(const stack_node_t *stack_node)
{
	assert(stack_node);

	return stack_node->data;
}

void *pop_stack(stack_node_t **stack_node)
{
	assert(stack_node);

	stack_node_t *prev = *stack_node;
	void *data = prev->data;
	*stack_node = (*stack_node)->next;
	free(prev);

	return data;
}

void delete_stack(stack_node_t **stack_node)
{
	assert(stack_node);

	while (*stack_node) {
		free(pop_stack(stack_node));
	}
}

int push_stack(stack_node_t **stack_node, const void *data, size_t size) {
	assert(stack_node && data && size);

	stack_node_t *new_stack_node = malloc(sizeof *new_stack_node);
	if (!new_stack_node) {
		return EXIT_FAILURE;
	}
	if (!(new_stack_node->data = malloc(size))) {
		free(new_stack_node);
		return EXIT_FAILURE;
	}

	memcpy(new_stack_node->data, data, size);
	new_stack_node->next = *stack_node;
	*stack_node = new_stack_node;

	return EXIT_SUCCESS;
}
