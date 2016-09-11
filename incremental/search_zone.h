/*
 * =====================================================================================
 *
 *       Filename:  search_zone.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/06/2012 12:16:25
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#pragma once

#include "dlist.h"
#include "typedefs.h"

typedef struct sz_t sz_t;

/**
 * @brief Data structure to represent a search zone
 */
struct sz_t {
	cost_t * corner; ///< Array representing the local upper bound that defines the search zone
	ccost_p_t * points;
};

/**
 * @brief Initialize the management of search zones
 *
 * @param dimension  dimension of tyhe objective space
 * @param ideal_cost array that represents the cost of the ideal point
 */
void sz_init (int dimension, const cost_t * lb0, const cost_t * ub0);


/**
 * @brief    Instanciate a search region (dlist of search zones)
 *
 * @param ub upper bound for each objective function
 *
 * @return   the new search region (one search sone)
 */
dlist_t * sz_list_new (void);
void sz_list_free_and_null (dlist_t ** sz_list);

/**
 * @brief         Print a search region
 *
 * @param sz_list search region
 */
void sz_list_print (const dlist_t * sz_list);

void sz_print (const sz_t * sz);

/**
 * @brief Update a list of search zones (= search region) according to a cost vector.
 *
 * @param sz_list List of search zones
 * @param v       Cost vector
 *
 * @return Incremental hypervolume according to 'v'
 */
double sz_update (dlist_t ** sz_list, const cost_t * v);

/**
 * @brief Compute the hypervolume
 *
 * @param sz_list
 *
 * @return 
 */
double hypervolume (const dlist_t * sz_list);
