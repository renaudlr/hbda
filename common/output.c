#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "macros.h"
#include "output.h"

#define TH 0.0001


void fprintln_double_tab (const double * tab, FILE * file, int length) {
	for (int i=0; i<length; i++)
		fprintf (file, "%.2f ", tab[i]);
	fprintf (file, "\n");
}

void fprintln_long_double_tab (const long double * tab, FILE * file, int length) {
	for (int i=0; i<length; i++)
		fprintf (file, "%Le ", tab[i]);
	fprintf (file, "\n");
}

void fprint_double_tab (const double * tab, FILE * file, int length) {
	for (int i=0; i<length; i++)
		fprintf (file, "%.2f ", tab[i]);
}

void fprintln_int_tab (const int * tab, FILE * file, int length) {
	for (int i=0; i<length; i++)
		fprintf (file, "%d ", tab[i]);
	fprintf (file, "\n");
}

void fprintln_cost_tab (const cost_t * tab, FILE * file, int length) {
	for (int i=0; i<length; i++)
		fprintf (file, "%.2f ", tab[i]);
	fprintf (file, "\n");
}

void fprintln_int_tab_ix (const int * tab, FILE * file, int length) {
	for (int i=0; i<length; i++)
		fprintf (file, "[%d]: %d ", i, tab[i]);
	fprintf (file, "\n");
}

void fprint_int_tab (const int * tab, FILE * file, int length) {
	for (int i=0; i<length; i++) {
		fprintf (file, "%d", tab[i]);
		if (i <length-1)
		fprintf (file, " ");
	}
}

void fprint_cost_tab (const cost_t * tab, FILE * file, int length) {
	for (int i=0; i<length; i++) {
		fprintf (file, "%.2f", tab[i]);
		if (i <length-1)
		fprintf (file, " ");
	}
}

void println_edge_weights (const double * weightvec, const int *sorted_edges, const int ** graph, int m, int p) {
	double weight, lastweight = 0;
	for (int i=0; i<m; i++, weight = 0) {
		for (int j=0; j<p; j++)
			weight += weightvec[j] * graph[sorted_edges[i]][2+j];
		if (i == 0) {
			printf ("%3f: [%d", weight, sorted_edges[i]);
			lastweight = weight;
		}
		else if (weight >= lastweight-TH && weight <= lastweight+TH)
			printf (", %d", sorted_edges[i]);
		else {
			printf ("]\n%3f: [%d", weight, sorted_edges[i]);
			lastweight = weight;
		}
	}
	printf ("]\n");
}

void println_wcost (const double * weightvec, const int * costvec, int p) {
	double wcost = 0;
	for (int j = 0; j<p; j++)
		wcost += weightvec[j] * costvec[j];
	printf("%f\n", wcost);
}
