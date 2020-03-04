#include "vector.h"
#include <assert.h>
#include <stdlib.h>

#define VECTOR_INIT_SIZE 8
#define VECTOR_GROW_FACTOR 2

vector_t *new_vector()
{
	vector_t *vector = malloc(sizeof (vector_t));
	if (!vector) {
		return NULL;
	}

	vector->data = malloc(VECTOR_INIT_SIZE * sizeof (void *));
	if (!vector->data) {
		free(vector);
		return NULL;
	}

	vector->size = 0;
	vector->capacity = VECTOR_INIT_SIZE;

	return vector;
}

void delete_vector(vector_t **p_vector)
{
	assert(p_vector);

//	while ((*p_vector)->size) {
//		free(pop_vector(*p_vector));
//	}

	free((*p_vector)->data);
	(*p_vector)->data = NULL;
	free(*p_vector);
	*p_vector = NULL;
}

int push_vector(vector_t *vector, void *data)
{
	if (vector->size == vector->capacity) {
		size_t new_capacity = vector->capacity * VECTOR_GROW_FACTOR;
		void **new_data = realloc(vector->data, new_capacity * sizeof (void *));
		if (!new_data) {
			return EXIT_FAILURE;
		}
		vector->data = new_data;
		vector->capacity = new_capacity;
	}

	vector->data[vector->size] = data;
	++vector->size;

	return EXIT_SUCCESS;
}

void *pop_vector(vector_t *vector)
{
	assert(vector && vector->size && vector->capacity && vector->data);

	return vector->data[--vector->size];
}

