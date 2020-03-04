#ifndef VECTOR_H_
#define VECTOR_H_

#include <stddef.h>

typedef struct vector {
	size_t size;
	size_t capacity;
	void **data;
} vector_t;

vector_t *new_vector();
void delete_vector(vector_t **p_vector);
int push_vector(vector_t *vector, void *data);
void *pop_vector(vector_t *vector);

#endif  // VECTOR_H_
