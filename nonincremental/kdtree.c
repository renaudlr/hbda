/*
 * =====================================================================================
 *
 *       Filename:  kdtree.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  15/11/2014 12:56:45
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include "typedefs.h"
#include "output.h"
#include "macros.h"

#include "kdtree.h"

static void populate_rec (kdnode_t ** node, cost_t ** pts, int start, int end, int dir, int dim);
void print_rec (const kdnode_t * node, int skip);
static int comp (const void * e1, const void * e2, void * arg);
static int comp_items (const void * e1, const void * e2);
static long height_rec (const kdnode_t * node);
static long nnodes_rec (const kdnode_t * node);
static long nitems_rec (const kdnode_t * node);
static long max_items_rec (const kdnode_t * node);
static void range_rec (kdnode_t *node, const cost_t * query, item_t ** result, int dim);
static void find_dom (item_t ** list, const cost_t * query, item_t ** result, int dim);
static void remove_items_rec (kdnode_t * node, item_t ** list);
static void free_rec (kdnode_t *node);
static inline void item_free (item_t * item);
static inline int nitems (const item_t * list);
static void print_items_rec (const kdnode_t * node, int dim);

kdtree_t * kdtree_new (int dim) {
	kdtree_t * result = calloc (1, sizeof (kdtree_t));
	if (result != NULL)
		result->dim = dim;
	return result;
}

void kdtree_free (kdtree_t * tree) {
	assert (tree != NULL);
	free_rec (tree->root);
	free (tree);
}

void kdtree_populate (kdtree_t * tree, cost_t ** points, int length) {
	cost_t ** pts = malloc (length * sizeof (ccost_p_t));
	assert (pts != NULL);
	memcpy (pts, points, length * sizeof (ccost_p_t));
	populate_rec (&(tree->root), pts, 0, length-1, -1, tree->dim);
	free (pts);
}

void kdtree_insert (kdtree_t * tree, cost_t * point, cost_t ** contrib) {
	bool inserted = false;
	kdnode_t * current = tree->root;
	item_t * new_item = malloc (sizeof (item_t));
	assert (new_item != NULL);
	new_item->point = point, new_item->contrib = contrib;
	while (!inserted) {
		/*if (better_pt_eq (point[current->dir], current->key, point, current->point)) {*/
		if (better_eq (point[current->dir], current->key)) {
			if (current->left != NULL)
				current = current->left;
			else {
				new_item->next = current->list_left;
				current->list_left = new_item;
				inserted = true;
			}
		}
		else {
			if (current->right != NULL)
			 	current = current->right;
			else {
				new_item->next = current->list_right;
				current->list_right = new_item;
				inserted = true;
			}
		}
	}
}

void kdtree_insert_item (kdtree_t * tree, item_t * item) {
	bool inserted = false;
	kdnode_t * current = tree->root;
	cost_t * point = item->point;
	while (!inserted) {
		/*if (better_pt_eq (point[current->dir], current->key, point, current->point)) {*/
		if (better_eq (point[current->dir], current->key)) {
			if (current->left != NULL)
				current = current->left;
			else {
				item->next = current->list_left;
				current->list_left = item;
				inserted = true;
			}
		}
		else {
			if (current->right != NULL)
			 	current = current->right;
			else {
				item->next = current->list_right;
				current->list_right = item;
				inserted = true;
			}
		}
	}
}

item_t * kdtree_range (kdtree_t * tree, const cost_t * query) {
	item_t * result = NULL;

	range_rec (tree->root, query, &result, tree->dim);

	return result;
}

void kdtree_print (const kdtree_t * tree) {
	assert (tree != NULL);
	print_rec (tree->root, 0);
}

long kdtree_height (const kdtree_t *tree) {
	return height_rec (tree->root);
}

long kdtree_nnodes (const kdtree_t *tree) {
	return nnodes_rec (tree->root);
}

long kdtree_nitems (const kdtree_t *tree) {
	return nitems_rec (tree->root);
}

long kdtree_max_items (const kdtree_t *tree) {
	return max_items_rec (tree->root);
}

void populate_rec (kdnode_t ** node, cost_t ** pts, int start, int end, int dir, int dim) {
	int new_dir = (dir+1)%dim;
	int med = (start+end)/2;
	qsort_r (pts+start, end-start+1, sizeof (cost_t*), comp, &new_dir);
	*node = calloc (1, sizeof (kdnode_t));
	assert (*node != NULL);
	(*node)->dir = new_dir;
	(*node)->key = pts[med][new_dir];
	(*node)->point = pts[med];
	if (med-1 >= start)
		populate_rec (&((*node)->left), pts, start, med-1, new_dir, dim);
	if (end >= med+1)
		populate_rec (&((*node)->right), pts, med+1, end, new_dir, dim);
}

item_t * kdtree_remove_items (kdtree_t * tree) {
	item_t * result = NULL;

	remove_items_rec (tree->root, &result);

	return result;
}

void range_rec (kdnode_t *node, const cost_t * query, item_t ** result, int dim) {
	/*if (better_pt (query[node->dir], node->key, query, node->point)) {*/
	if (better (query[node->dir], node->key)) {
		if (node->left == NULL)
			find_dom (&(node->list_left), query, result, dim);
		else
			range_rec (node->left, query, result, dim);
	}
	if (node->right == NULL)
		find_dom (&(node->list_right), query, result, dim);
	else
		range_rec (node->right, query, result, dim);
}

void find_dom (item_t ** list, const cost_t * query, item_t ** result, int dim) {
	item_t * current = *list, *current_prev = NULL, *move;
	bool insert;
	while (current != NULL) {
		insert = true;
		for (int j = 0; j < dim; j++)
			/*if (worse_pt_eq (query[j], current->point[j], query, current->contrib[j])) {*/
			if (worse_eq (query[j], current->point[j])) {
				insert = false;
				break;
			}
		if (insert) {
			if (current_prev == NULL)
				*list = current->next;
			else
				current_prev->next = current->next;
			move = current;
			current = current->next;
			move->next = *result;
			*result = move;
		}
		else {
			current_prev = current;
			current = current->next;
		}
	}
}

void remove_items_rec (kdnode_t * node, item_t ** list) {
	item_t * move;
	if (node == NULL)
		return;
	while (node->list_left != NULL) {
		move = node->list_left;
		node->list_left = node->list_left->next;
		move->next = *list;
		*list = move;
	}
	while (node->list_right != NULL) {
		move = node->list_right;
		node->list_right = node->list_right->next;
		move->next = *list;
		*list = move;
	}
	remove_items_rec (node->left, list), remove_items_rec (node->right, list);
}

void free_rec (kdnode_t *node) {
	kdnode_t * node_left, * node_right;
	item_t * current, * del;
	if (node == NULL)
		return;
	node_left = node->left;
	node_right = node->right;
	current = node->list_left;
	while (current != NULL) {
		del = current;
		current = current->next;
		item_free (del);
	}
	current = node->list_right;
	while (current != NULL) {
		del = current;
		current = current->next;
		item_free (del);
	}
	free (node);
	free_rec (node_left), free_rec (node_right);
}

void item_free (item_t * item) {
	free (item->point);
	free (item->contrib);
	free (item);
}

int nitems (const item_t * list) {
	int result = 0;
	while (list != NULL) {
		result++;
		list = list->next;
	}
	return result;
}

void print_rec (const kdnode_t * node, int skip) {
	if (node == NULL)
		return;
	for (int i = 0; i<skip; i++)
		printf("  ");
	printf("(%d: %.2f)", node->dir, node->key);
	if (node->list_left != NULL)
		printf("+%dl", nitems(node->list_left));
	if (node->list_right != NULL)
		printf("+%dr", nitems(node->list_right));
	printf("\n");
	print_rec (node->left, skip+1);
	print_rec (node->right, skip+1);
}

long height_rec (const kdnode_t * node) {
	long height_left, height_right;

	if (node == NULL)
		return 0;
	else {
		height_left = height_rec (node->left);
		height_right = height_rec (node->right);
		return 1 + (height_left >= height_right ? height_left : height_right);
	}
}

long nnodes_rec (const kdnode_t * node) {
	if (node == NULL)
		return 0;
	else
		return 1 + nnodes_rec (node->left) + nnodes_rec (node->right);
}

long nitems_rec (const kdnode_t * node) {
	if (node == NULL)
		return 0;
	else
		return nitems (node->list_left) + nitems (node->list_right) 
			+ nitems_rec (node->left) + nitems_rec (node->right);
}

long max_items_rec (const kdnode_t * node) {
	long current_max, current_max_left, current_max_right, max_items_left, max_items_right;
	if (node == NULL)
		return 0;
	else {
		max_items_left = max_items_rec (node->left);
		max_items_right = max_items_rec (node->right);
		current_max_left = nitems(node->list_left);
		current_max_right = nitems(node->list_right);
		current_max = current_max_left >= current_max_right ? current_max_left : current_max_right;
		current_max = current_max >= max_items_left ? current_max : max_items_left;
		current_max = current_max >= max_items_right ? current_max : max_items_right;
		return current_max;
	}
}

int comp (const void * e1, const void * e2, void * arg) {
	int* dir = (int*) arg;
	const cost_t * v1 = *((const cost_t**) e1);
	const cost_t * v2 = *((const cost_t**) e2);
	/*if (better_pt (v1[*dir], v2[*dir], v1, v2))*/
	if (better (v1[*dir], v2[*dir]))
		return -1;
	else if (v1 == v2)
		return 0;
	else
		return 1;
}

int comp_items (const void * e1, const void * e2) {
	const item_t * i1 = ((const item_t*) e1);
	const item_t * i2 = ((const item_t*) e2);
	if (i1->point[0] < i2->point[0])
		return -1;
	else if (i1->point[0] > i2->point[0])
		return 1;
	else
		return 0;
}

void kdtree_print_items (const kdtree_t * tree, int dim) {
	printf("kd-tree %p:\n", tree);
	print_items_rec (tree->root, dim);
}

void print_items_rec (const kdnode_t * node, int dim) {
	const item_t * current_item;

	if (node == NULL) return;

	print_items_rec (node->left, dim);
 
	current_item = node->list_left;
	while (current_item != NULL) {
		println_double_tab (current_item->point, dim);
		current_item = current_item->next;
	}

	current_item = node->list_right;
	while (current_item != NULL) {
		println_double_tab (current_item->point, dim);
		current_item = current_item->next;
	}

	print_items_rec (node->right, dim);
}
