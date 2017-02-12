/*
 * util.c
 *
 * Status: C
 *
 *  Created on: Feb 3, 2017
 *      Author: Fred
 *
 *      Contains helper function implementations
 */

#include "util.h"

/* Simple function to check if input is changed from last_value by at least threshold amount*/
BOOL int16U_changed_by_threshold(INT16U input, INT16U last_value,
		INT16U threshold) {

	if (input > last_value && input - last_value > threshold)
		return TRUE;

	if (last_value > input && last_value - input > threshold)
		return TRUE;

	return FALSE;
}

/* Simple function to check if two inputs differ by a certain percent
 * percent is input as an integer and it's accurate to
 * for example, to check 10 percent, input number 10
 * the actual accuracy is defined by PERCENT_DIFF_ACCURACY
 */
BOOL int16U_differ_by_percent(INT16U src1, INT16U src2, INT16U percent,
		INT16U accuracy) {

	INT16U larger;
	INT16U smaller;

	if (src1 > src2) {
		larger = src1;
		smaller = src2;
	} else {
		larger = src2;
		smaller = src1;
	}
	if (larger == 0)
		return TRUE;
	if (smaller == 0) {
		if (larger <= percent)
			return TRUE;
		else
			return FALSE;
	}
	INT32U diff = larger - smaller;
	INT32U avg = (larger + smaller) / 2;
	INT32U result = (diff * accuracy) / avg;
	INT32U threshold = percent * accuracy / 100;
	if (result <= threshold)
		return FALSE;
	else
		return TRUE;
}

