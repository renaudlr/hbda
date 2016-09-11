/*
 * =====================================================================================
 *
 *       Filename:  util.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  19/11/2014 14:26:23
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#include "typedefs.h"
#include "macros.h"

#include "util.h"


int vcomp (const void * v1, const void * v2, void * j) {
	const cost_t * iv1 = * ((const cost_t **) v1), * iv2 = *((const cost_t **) v2);
	int * ij = (int*) j;
	return (diff (iv1[*ij], iv2[*ij]));
}
