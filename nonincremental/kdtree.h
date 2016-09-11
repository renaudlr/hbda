/*
 * =====================================================================================
 *
 *       Filename:  kdtree.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  15/11/2014 12:43:17
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

typedef struct kdnode_t kdnode_t;
typedef struct kdtree_t kdtree_t;
typedef struct item_t item_t;

/**
 * @brief Representation of a kdtree node
 */
struct kdnode_t {
	cost_t key;
	int dir;
	const cost_t * point;
	item_t * list_left, * list_right;
	kdnode_t * left, * right;
};

/**
 * @brief Representation of a kdtree
 */
struct kdtree_t {
	kdnode_t * root;
	int dim;
};

/**
 * @brief iList item representing a point attached to a kdtree leave
 */
struct item_t {
	cost_t * point;
	cost_t ** contrib;
	struct item_t * next;
};

kdtree_t * kdtree_new (int dim);
void kdtree_free (kdtree_t * tree);
/**
 * @brief Initializes a kdtree
 *
 * @param tree   A kdtree
 * @param points An array of points
 * @param length The array length
 */
void kdtree_populate (kdtree_t * tree, cost_t ** points, int length);
item_t * kdtree_remove_items (kdtree_t * tree);
void kdtree_print (const kdtree_t *tree);
long kdtree_height (const kdtree_t *tree);
long kdtree_nnodes (const kdtree_t *tree);
long kdtree_nitems (const kdtree_t *tree);
long kdtree_max_items (const kdtree_t *tree);
void kdtree_insert (kdtree_t * tree, cost_t * point, cost_t ** contrib);
void kdtree_insert_item (kdtree_t * tree, item_t *e);
/**
 * @brief Performs a range query on a kdtree
 *
 * @param tree  The kdtree
 * @param query The query point
 *
 * @return A list of points found
 */
item_t * kdtree_range (kdtree_t * tree, const cost_t * query);
item_t * kdtree_range_sup (kdtree_t * tree, const cost_t * query);
void kdtree_print_items (const kdtree_t * tree, int dim);
