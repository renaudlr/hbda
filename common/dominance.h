#pragma once

#include <stdbool.h>
#include "typedefs.h"

bool dom (const cost_t * v1, const cost_t * v2, int p);
bool weak_dom     ( const cost_t * v1, const cost_t * v2, int p);
bool strong_dom   ( const cost_t * v1, const cost_t * v2, int p);
int bi_weak_dom (const cost_t * v1, const cost_t * v2, int dim);
int bi_weak_dom_restrict (const cost_t * v1, const cost_t * v2, uint dims);
#define SPDOM_ND -1
#define SPDOM_D  -2
int special_dom (const cost_t * v1, const cost_t * v2, int p);
double dominance_get_spdom_comp (void);
double dominance_get_other_comp (void);
