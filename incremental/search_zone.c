/*
 * =====================================================================================
 *
 *       Filename:  search_zone.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/06/2012 12:16:09
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

/* #####   HEADER FILE INCLUDES   ################################################### */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "macros.h"
#include "dlist.h"
#include "dominance.h"
#include "output.h"
#include "search_zone.h"


/* #####   MACROS  -  LOCAL TO THIS SOURCE FILE   ################################### */

/* #####   TYPE DEFINITIONS  -  LOCAL TO THIS SOURCE FILE   ######################### */

/* #####   DATA TYPES  -  LOCAL TO THIS SOURCE FILE   ############################### */
/* #####   VARIABLES  -  LOCAL TO THIS SOURCE FILE   ################################ */
static int dim = 0;
static const cost_t * lb, * ub;

/* #####   PROTOTYPES  -  LOCAL TO THIS SOURCE FILE   ############################### */
static inline sz_t * sz_new (const cost_t * corner);
static inline void sz_free (sz_t * del);
static double box_volume (const sz_t * sz);
static int sz_compar (const void * x, const void * y);
static cost_t ** dummies;

/* #####   FUNCTION DEFINITIONS  -  EXPORTED FUNCTIONS   ############################ */
dlist_t * sz_list_new (void) {
	dlist_t * result = NULL;
	sz_t * sz_ini = sz_new (ub);
	for (int j=0; j<dim; j++) {
		sz_ini->points[j] = dummies[j];
	}
	dlist_add (&result,sz_ini);
	return result;
}

void sz_list_free_and_null (dlist_t ** sz_list) {
	dlist_free_and_null ((*sz_list), sz_free);
}

void sz_init (int dimension, const cost_t * lb0, const cost_t * ub0) {
	dim = dimension;
	lb = lb0, ub = ub0;
	dummies = malloc (dim * sizeof (cost_t*));
	for (int j=0; j<dim; j++) {
		dummies[j] = malloc (dim * sizeof (cost_t));
		for (int k = 0; k < dim; k++)
			dummies[j][k] = (k == j) ? ub[k] : lb[k];
	}
}

void sz_list_print (const dlist_t * sz_list) {
	dlist_print (sz_list, sz_print);
}

/* #####   FUNCTION DEFINITIONS  -  LOCAL TO THIS SOURCE FILE   ##################### */
sz_t * sz_new (const cost_t * corner) {
	sz_t * result = malloc (sizeof (sz_t));
	result->corner = malloc (dim * sizeof (cost_t));
	memcpy (result->corner, corner, dim*sizeof (cost_t));
	result->points = malloc (dim * sizeof (ccost_p_t));
	return result;
}


void sz_free (sz_t * del) {
	free (del->corner);
	free (del->points);
	free (del);
}

void sz_print (const sz_t * sz) {
	print_cost_tab (sz->corner,dim);
	printf("\n");
}

/* To Sort in decreasing order */
int sz_compar (const void * x, const void * y) {
	const sz_t * x_sz = (const sz_t *) x;
	const sz_t * y_sz = (const sz_t *) y;
	if (better (x_sz->corner[0], y_sz->corner[0]))
		return 1;
	else if (worse (x_sz->corner[0], y_sz->corner[0]))
		return -1;
	else
		return 0;
}

double sz_update (dlist_t ** sz_list, const cost_t * v) {
	int j, k;
	dlist_t * e_sz1;
	sz_t * sz_p1, *sz_p2;
	bool compute_child_sz;
	ccost_p_t pt, * pts1, *pts2;

	double result = 0;
	bool first = true;
	dlist_t * e_first;

	e_sz1 = *sz_list;
	while (e_sz1 != NULL) {
		sz_p1 = e_sz1->data;
		/* Assumes that the local upper bounds are sorted in decreasing order*/
		if (worse_eq (v[0], sz_p1->corner[0]))
			break;
		if (strong_dom (v, sz_p1->corner, dim)) 
		{
			result -= box_volume (sz_p1);
			for (j = 0; j < dim; j++) {
				compute_child_sz = true;
				pts1 = sz_p1->points;
				for (k = 0; k < dim && compute_child_sz; k++) {
					if (k == j)
						continue;
					pt = pts1[k];
					if (better (v[j], pt[j]))
						compute_child_sz = false;
				}
				if (compute_child_sz) {
					sz_p2 = sz_new (sz_p1->corner);
					sz_p2->corner[j] = v[j];
					pts2 = sz_p2->points;
					memcpy (pts2, pts1, dim*sizeof (ccost_p_t));
					pts2[j] = v;
					result += box_volume (sz_p2);
					if (j == 0 && first) {
						e_first = dlist_add_ordered (&(e_sz1->next), sz_p2, sz_compar);
						first = false;
					}
					else if (j == 0)
						dlist_add_ordered (&(e_first->next), sz_p2, sz_compar);
					else
						dlist_add_ordered (&(e_sz1->next), sz_p2, sz_compar);
				}
			}
			sz_free (sz_p1);
			e_sz1 = dlist_remove_elem (sz_list, e_sz1);
		}
		else {
			e_sz1 = e_sz1->next;
		}
	}
	return (result);
}

double box_volume (const sz_t * sz) {
	ccost_p_t * pts = sz->points;
	cost_t z_worst;
	double result = diff (ub[0], pts[0][0]) * (double) diff (pts[1][1], pts[0][1]);
	for (int i = 2; i<dim; i++) {
		z_worst = pts[0][i];
		for (int j = 1; j<i; j++)
			if (worse (pts[j][i], z_worst)) z_worst = pts[j][i];
		result *= diff (pts[i][i], z_worst);
	}
	return result;
}

double hypervolume (const dlist_t * sz_list) {
	double result = 0;
	const dlist_t * e_sz;
	_foreach (e_sz, sz_list) {
		result += box_volume(e_sz->data);
	}
	return result;
}
