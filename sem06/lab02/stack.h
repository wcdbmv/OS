#ifndef STACK_H_
#define STACK_H_

#include <stdbool.h>
#include <stddef.h>

typedef struct stack_node_t {
	void *data;
	struct stack_node_t *next;
} stack_node_t;

const void *top_stack(const stack_node_t *stack_node);
void *pop_stack(stack_node_t **p_stack_node);
void delete_stack(stack_node_t **p_stack_node);
int push_stack(stack_node_t **p_stack_node, const void *data, size_t size);
bool empty_stack(const stack_node_t *stack_node_t);

#endif  // STACK_H_
