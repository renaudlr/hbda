#pragma once

#include "typedefs.h"

void fprintln_double_tab (const double * tab, FILE * file, int length);
void fprintln_long_double_tab (const long double * tab, FILE * file, int length);
void fprint_double_tab (const double * tab, FILE * file, int length);
void fprintln_int_tab (const int * tab, FILE * file, int length);
void fprintln_cost_tab (const cost_t * tab, FILE * file, int length);
void fprintln_int_tab_ix (const int * tab, FILE * file, int length);
void fprint_int_tab (const int * tab, FILE * file, int length);
void fprint_cost_tab (const cost_t * tab, FILE * file, int length);
void println_edge_weights (const double * weightvec, const int *sorted_edges, const int ** graph, int m, int p);
void println_wcost (const double * weightvec, const int * costvec, int p);
static inline void println_double_tab (const double * tab, int length) {
	fprintln_double_tab (tab, stdout, length);
}
static inline void println_long_double_tab (const long double * tab, int length) {
	fprintln_long_double_tab (tab, stdout, length);
}
static inline void print_double_tab (const double * tab, int length) {
	fprint_double_tab (tab, stdout, length);
}
static inline void println_int_tab (const int * tab, int length) {
	fprintln_int_tab (tab, stdout, length);
}
static inline void println_cost_tab (const cost_t * tab, int length) {
	fprintln_cost_tab (tab, stdout, length);
}
static inline void println_int_tab_ix (const int * tab, int length) {
	fprintln_int_tab_ix (tab, stdout, length);
}
static inline void print_int_tab (const int * tab, int length) {
	fprint_int_tab (tab, stdout, length);
}
static inline void print_cost_tab (const cost_t * tab, int length) {
	fprint_cost_tab (tab, stdout, length);
}
