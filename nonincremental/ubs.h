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

/**
 * @brief Structure to represent an upper bound set
 */
typedef struct ubs_t ubs_t;
struct ubs_t;

/**
 * @brief Creates a new upper bound set
 *
 * @param dim Dimension
 *
 * @return 
 */
ubs_t * ubs_new (int dim);
/**
 * @brief Initializes a newly created upper bound set
 *
 * @param ubs      Upper bound set to be initialized
 * @param points   Whole set of points (to initialize the underlying kdtree structure)
 * @param npoints  Number of points in 'points'
 * @param lb       Lower bound point
 * @param ub       Upper bound point
 */
void ubs_init (ubs_t * ubs, cost_t ** points, int npoints, const cost_t * lb, const cost_t * ub);
/**
 * @brief Free an upper bound set
 *
 * @param ubs The upper bound set
 */
void ubs_free_and_null (ubs_t ** ubs);
/**
 * @brief Updates an upper bound set with a point (must belong to the set of points used in initialization)
 *
 * @param ubs The upper bound set
 * @param v   The point to update with
 */
void ubs_update (ubs_t * ubs, cost_t * v);
/**
 * @brief Closes the unbounded boxes (at the end)
 *
 * @param ubs The upper bound set
 */
void ubs_close_boxes (ubs_t * ubs);
/**
 * @brief Isn't it explicit?
 *
 * @param ubs
 *
 * @return 
 */
double ubs_get_hypervolume (const ubs_t * ubs);
/**
 * @brief Print some stats about an upper bound set
 *
 * @param ubs The upper bound set
 */
void ubs_print_stats (ubs_t * ubs);
