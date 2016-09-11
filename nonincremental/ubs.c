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
#include "int_fstack.h"
#include "dominance.h"
#include "output.h"
#include "kdtree.h"

#include "ubs.h"

/* #####   MACROS  -  LOCAL TO THIS SOURCE FILE   ################################### */

/* #####   TYPE DEFINITIONS  -  LOCAL TO THIS SOURCE FILE   ######################### */
struct ubs_t {
	int dim;
	cost_t * tmp_lub;
	cost_t ** tmp_points;
	const cost_t * ref;
	/*cost_t ** dummies;*/
	kdtree_t * tree;
	int_fstack_t * sub;
	double hypervolume;
};

/* #####   DATA TYPES  -  LOCAL TO THIS SOURCE FILE   ############################### */
/* #####   VARIABLES  -  LOCAL TO THIS SOURCE FILE   ################################ */

/* #####   PROTOTYPES  -  LOCAL TO THIS SOURCE FILE   ############################### */
static inline double box_hypervolume (const ubs_t * ubs, cost_t ** contrib, cost_t closing);

/* #####   FUNCTION DEFINITIONS  -  EXPORTED FUNCTIONS   ############################ */
ubs_t * ubs_new (int dim) {
	ubs_t * result = malloc (sizeof (ubs_t));
	result->dim = dim;
	result->tree = kdtree_new (dim);
	result->sub = int_fstack_new (dim);
	result->hypervolume = 0;

	return result;
}

void ubs_init (ubs_t * ubs, cost_t ** points, int npoints, const cost_t * lb, const cost_t * ub) {
	cost_t ** dummies = malloc ((ubs->dim) * sizeof (cost_t*));
	for (int j=0; j<ubs->dim; j++) {
		dummies[j] = malloc ((ubs->dim+1) * sizeof (cost_t));
		for (int k = 0; k < ubs->dim+1; k++)
			dummies[j][k] = (k == j) ? ub[k] : lb[k];
	}
	kdtree_populate (ubs->tree, points, npoints);

	cost_t * first_corner = malloc (ubs->dim * sizeof (cost_t));
	memcpy (first_corner, ub, ubs->dim*sizeof (cost_t));
	kdtree_insert (ubs->tree, first_corner, dummies);

	ubs->ref = ub;
}

void ubs_free_and_null (ubs_t ** ubs) {
	int_fstack_free_and_null (&((*ubs)->sub));
	kdtree_free ((*ubs)->tree);
	free (*ubs);
	*ubs = NULL;
}

/* Assumes that the points are sorted */
void ubs_compute (ubs_t * ubs, cost_t ** points, int length) {
	for (int i = 0; i<length; i++)
		ubs_update (ubs, points[i]);
}

/* #####   FUNCTION DEFINITIONS  -  LOCAL TO THIS SOURCE FILE   ##################### */
/* Note: 'v' cannot be const but what it points to should not be changed either. */
void ubs_update (ubs_t * ubs, cost_t * v) {
	int j, k;
	bool compute_child_sz;
	cost_t *pt;

	item_t * visible;
	item_t * current, *current_next;

	visible = kdtree_range (ubs->tree, v);

	current = visible;
	while (current != NULL) {
		cost_t * current_corner = current->point;
		cost_t ** current_pts = current->contrib;

		/* Computation of the hypervolume */
		ubs->hypervolume += box_hypervolume (ubs, current->contrib, v[ubs->dim]);

		for (j = 0; j < ubs->dim; j++) {
			compute_child_sz = true;
			for (k = 0; k < ubs->dim && compute_child_sz; k++) {
				if (k == j)
					continue;
				pt = current_pts[k];
				if (better (v[j], pt[j]))
					compute_child_sz = false;
			}
			if (compute_child_sz) {
				int_fstack_push (ubs->sub, j);
			}
		}
		current_next = current->next;
		if (int_fstack_isempty (ubs->sub)) {
			free (current->point);
			free (current->contrib);
			free (current);
		}
		while (! int_fstack_isempty (ubs->sub)) {
			j = int_fstack_pop (ubs->sub);
			if (int_fstack_isempty (ubs->sub)) {
				current_corner[j] = v[j];
				current_pts[j] = v;
				kdtree_insert_item (ubs->tree, current);
			}
			else {
				ubs->tmp_lub = malloc (ubs->dim * sizeof (cost_t));
				ubs->tmp_points = malloc (ubs->dim * sizeof (ccost_p_t));
				memcpy (ubs->tmp_lub, current_corner, ubs->dim*sizeof (cost_t));
				ubs->tmp_lub[j] = v[j];
				memcpy (ubs->tmp_points, current_pts, ubs->dim*sizeof (ccost_p_t));
				ubs->tmp_points[j] = v;
				kdtree_insert (ubs->tree, ubs->tmp_lub, ubs->tmp_points);
			}
		}
		current = current_next;
	}
#ifdef UBS_DEBUG
	kdtree_print_items (ubs->tree, ubs->dim);
	printf("\n");
#endif
}

void ubs_close_boxes (ubs_t * ubs) {
	item_t * visible;
	item_t * current, *current_next;

	int count = 0;
	cost_t closing = ubs->ref[ubs->dim];

	visible = kdtree_remove_items (ubs->tree);

	current = visible;
	while (current != NULL) {
		count++;
		
		ubs->hypervolume += box_hypervolume (ubs, current->contrib, closing);

		current_next = current->next;
		free (current->point);
		free (current->contrib);
		free (current);

		current = current_next;
	}
}

double box_hypervolume (const ubs_t * ubs, cost_t ** contrib, cost_t closing) {
	double result = 0;

	cost_t z_worst;
	assert (ubs->dim > 1);
	result += diff (ubs->ref[0], contrib[0][0]) * (double) diff (contrib[1][1], contrib[0][1]);
	assert (result >= 0);
	for (int i = 2; i<ubs->dim; i++) {
		z_worst = contrib[0][i];
		for (int j = 1; j<i; j++)
			if (worse (contrib[j][i], z_worst)) {
				z_worst = contrib[j][i];
			}
		result *= diff (contrib[i][i], z_worst);
		assert (result >= 0);
	}
	z_worst = contrib[0][ubs->dim];
	for (int j = 1; j<ubs->dim; j++)
		if (worse (contrib[j][ubs->dim], z_worst)) {
			z_worst = contrib[j][ubs->dim];
		}
	result *= diff (closing, z_worst);
	assert (result >= 0);
	return result;
}

double ubs_get_hypervolume (const ubs_t * ubs) {
	return ubs->hypervolume;
}

void ubs_print_stats (ubs_t * ubs) {
	printf ("Number of nodes in the kd-tree: %ld\n", kdtree_nnodes (ubs->tree));
	printf ("Number of items in the kd-tree: %ld\n", kdtree_nitems (ubs->tree));
	printf ("Max. Number of items in the kd-tree: %ld\n", kdtree_max_items (ubs->tree));
	printf ("Height of the kd-tree: %ld\n", kdtree_height (ubs->tree));
	/*kdtree_print (ubs->tree);*/
}
