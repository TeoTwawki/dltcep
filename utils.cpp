#include "stdafx.h"
#include "utils.h"

// Greatest Common Divisor functions

// GCD of two integer numbers
int GCD (int q, int r) {
	if (q < 0) q = -q;
	if (r < 0) r = -r;

	while (r) {
		int r_new = q % r;
		q = r;
		r = r_new;
	}
	return q;
}

// GCD of shorts in an array
int GCD (const short* values, int count) {
	if (!count) return 0;

	int pos = 1,
		result = values[0];
	while (pos < count) {
		result = GCD (result, values[pos]);
		pos++;
	}
	return result;
}
