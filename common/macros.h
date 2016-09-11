#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>

#define UNDEF (-1)

#ifdef MAXIMIZE
#define better_eq(x, y) ((x) >= (y))
#define better(x, y) ((x) > (y))
#define worse_eq(x, y) ((x) <= (y))
#define worse(x, y) ((x) < (y))
#define enhance(x) ((x)+=DBL_MIN)
#define degrade(x) ((x)-=DBL_MIN)
#define diff(x, y) ((y)-(x))
#define unit_cube_ub (0.0)
#define unit_cube_lb (1.0)
#else
#define better_eq(x, y) ((x) <= (y))
#define better(x, y) ((x) < (y))
#define worse_eq(x, y) ((x) >= (y))
#define worse(x, y) ((x) > (y))
#define enhance(x) ((x)-=DBL_MIN)
#define degrade(x) ((x)+=DBL_MIN)
#define diff(x, y) ((x)-(y))
#define unit_cube_ub (1.0)
#define unit_cube_lb (0.0)
#endif
#define better_pt(x, y, px, py) (better((x), (y)) || ((x) == (y) && (px) < (py)))
#define better_pt_eq(x, y, px, py) (better((x), (y)) || ((x) == (y) && (px) <= (py)))
#define worse_pt(x, y, px, py) (worse((x), (y)) || ((x) == (y) && (px) > (py)))
#define worse_pt_eq(x, y, px, py) (worse((x), (y)) || ((x) == (y) && (px) >= (py)))

#define _free_and_null(ptr) (free(ptr),  (ptr) = NULL)

#define min(a, b) ((a)<(b)?(a):(b))
#define max(a, b) ((a)<(b)?(b):(a))
#define _abs(a) ((a)>0?(a):(-(a)))

#define _foreach(elem,list) for (elem = list; elem != NULL; elem = elem->next)
#define _apply_foreach(list, func, ...) { \
	typeof(list) elem; \
	_foreach (elem, list) func (elem, ##__VA_ARGS__); \
}
#define _eq(a, b) (((a) && (b)) || (!(a) && !(b)))

#define _b2s(boolean) ((boolean)?"true":"false")

#ifdef NDEBUG
#define _print_stat(statement) ((void)(0))
#define _print_stat_if_true(statement) ((void)(0))
#define _dumpf(x, fmt) ((void)(0))
#else
#define _print_stat(statement) printf("%s: %s\n", #statement, _b2s(statement))
#define _print_stat_if_true(statement) ((statement)? printf("%s\n", #statement):false)
#define _dumpf(x, fmt) printf("%s: " fmt, #x, x)
#endif

static inline void error (const char * msg) {
	fprintf (stderr, "Error: %s\n", msg);
	exit (EXIT_FAILURE);
}

static inline void * memcpy_or_null (const void * src, size_t n) {
	void * result;
	if (src == NULL)
		result = NULL;
	else {
		result = malloc (n);
		result = memcpy (result, src, n);
	}
	return result;
}

static inline void * memcpy_or_calloc (const void * src, size_t n, size_t size) {
	void * result;
	if (src == NULL)
		result = calloc (n, size);
	else {
		result = malloc (n*size);
		result = memcpy (result, src, n*size);
	}
	return result;
}

static inline int * new_int_tab (int length) {
	int * result = malloc (length * sizeof (int));
	for (int i = 0; i<length; i++) {
		result[i] = UNDEF;
	}
	return result;
}

static inline int arg_min_abs_diff (const int * tab, int length, int ref) {
	int i, result = 0;
	int val, cur_val;
	val = _abs(ref-tab[0]);
	for (i=1; i<length; i++) {
		if (tab[i] == -1)
			continue;
		cur_val = _abs(ref-tab[i]);
		if (cur_val < val)
			val = cur_val, result = i;
		if (val == 0)
			break;
	}
	return result;
}

static inline int int_scalar_product (const int * v1, const int * v2, int p) {
	int result = 0;
	for (int j = 0; j<p; j++)
		result += v1[j] * v2[j];
	return result;
}

static inline double intdbl_scalar_product (const int * v1, const double * v2, int p) {
	double result = 0;
	for (int j = 0; j<p; j++)
		result += v1[j] * v2[j];
	return result;
}
