#pragma once

typedef struct int_fstack_t int_fstack_t;
struct int_fstack_t {
	int * data;
	int top;
	int size;
};

void int_fstack_push (int_fstack_t * stack, int val);
int int_fstack_pop (int_fstack_t * stack);
int_fstack_t * int_fstack_new (int size);
void int_fstack_free_and_null (int_fstack_t ** stack);
#define int_fstack_isempty(stack) ((stack)->top == -1)
#define int_fstack_top(stack) ((stack)->data[(stack)->top])
#define int_fstack_empty(stack) ((stack)->top = -1)
