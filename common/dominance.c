#include <assert.h>
#include <stdbool.h>

#include "dominance.h"
#include "macros.h"
#include "output.h"

static double spdom_comp = 0;
static double other_comp = 0;

bool dom (const cost_t * v1, const cost_t * v2, int p) {
	int i;
	bool one_ineq = false;
	for (i=0; i < p; i++) {
#ifdef DOM_COUNT_COMP
		other_comp++;
#endif
		if (worse (v1[i], v2[i]))
			return false;
		else if (better (v1[i], v2[i]))
			one_ineq = true;
	}
	return one_ineq;
}

bool weak_dom (const cost_t * v1, const cost_t * v2, int p) {
	int i;
	for (i=0; i < p; i++) {
#ifdef DOM_COUNT_COMP
		other_comp++;
#endif
		if (worse (v1[i], v2[i]))
			return false;
	}
	return true;
}

bool strong_dom (const cost_t * v1, const cost_t * v2, int p) {
	int i;
	for (i=0; i < p; i++) {
#ifdef DOM_COUNT_COMP
		spdom_comp++;
#endif
		if (worse_eq(v1[i], v2[i]))
			return false;
	}
	return true;
}

int bi_weak_dom (const cost_t * v1, const cost_t * v2, int dim) {
	int i = 0;
	while (v1[i] == v2[i]) {
		i++;
		if (i == dim)
			return 1;
	}
	// i < first + p
	if (better (v1[i], v2[i])) {
		// return 0 or 1
		for ( ; i<dim; i++)
			if (worse (v1[i], v2[i]))
				return 0;
		return 1;
	}
	else {
		// return 0 or -1
		for ( ; i<dim; i++)
			if (better (v1[i], v2[i]))
				return 0;
		return -1;
	}
}

int bi_weak_dom_restrict (const cost_t * v1, const cost_t * v2, uint dims) {
	assert (dims > 0);
	uint k = ffs(dims);
	dims >>= k;
	int i = k-1;
	/*printf("v1; ");
	println_double_tab (v1, 4);
	printf("v2; ");
	println_double_tab (v2, 4);*/
	while (v1[i] == v2[i]) {
		if (dims <= 0)
			return -1;
		k = ffs(dims); dims >>= k;
		i += k;
	}
	if (better (v1[i], v2[i])) {
		// return 0 or 1
		while (dims > 0)  {
			k = ffs(dims); dims >>= k;
			i += k;
			if (worse (v1[i], v2[i]))
				return 0;
		}
		return 1;
	}
	else {
		// return 0 or -1
		while (dims > 0)  {
			k = ffs(dims); dims >>= k;
			i += k;
			if (better (v1[i], v2[i]))
				return 0;
		}
		return -1;
	}
}

/**
 * @brief    Special dominance
 *
 * @param v1
 * @param v2
 * @param p
 *
 * @return   SPDOM_D if v1 < v2, i if v1[i] == v2[i] and v1[-i] < v2[-i], SPDOM_ND otherwise
 */
int special_dom (const cost_t * v1, const cost_t * v2, int p) {
	int eq_index = SPDOM_D;
	for (int i = 0; i<p; i++) {
#ifdef DOM_COUNT_COMP
		spdom_comp++;
#endif
		if (v1[i] == v2[i]) {
			if (eq_index == SPDOM_D)
				eq_index = i;
			else
				return SPDOM_ND;
		}
		else {
#ifdef DOM_COUNT_COMP
			spdom_comp++;
#endif
			if (worse(v1[i], v2[i]))
				return SPDOM_ND;
		}
	}
	return eq_index;
}

double dominance_get_spdom_comp (void) {
	return spdom_comp;
}

double dominance_get_other_comp (void) {
	return other_comp;
}
