#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "int_fstack.h"

void int_fstack_push (int_fstack_t * stack, int val) {
	if (stack->top+1 >= stack->size)
		printf("%d >= %d\n", stack->top+1, stack->size);
	assert (stack->top+1 < stack->size);
	stack->data[++(stack->top)] = val;
}

int int_fstack_pop (int_fstack_t * stack) {
	assert (stack->top >= 0);
	return stack->data[(stack->top)--];
}

int_fstack_t * int_fstack_new (int size) {
	int_fstack_t * result = (int_fstack_t *) malloc (sizeof (int_fstack_t));
	assert (result != NULL);
	result->data = malloc (size * sizeof (int));
	assert (result->data != NULL);
	result->size = size;
	result->top = -1;
	return result;
}

void int_fstack_free_and_null (int_fstack_t ** stack) {
	free ((*stack)->data);
	free ((*stack));
	*stack = NULL;
}
