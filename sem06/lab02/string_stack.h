#ifndef STRING_STACK_H_
#define STRING_STACK_H_

#include "stack.h"

int push_string(stack_node_t **stack_node, const char *string);
char *pop_string(stack_node_t **stack_node);
const char *top_string(const stack_node_t *stack_node);

#endif  // STRING_STACK_H_
