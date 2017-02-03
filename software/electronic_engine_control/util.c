/*
 * util.c
 *
 *  Created on: Feb 3, 2017
 *      Author: Fred
 *
 *      Contains helper function implementations
 */

#include "util.h";

/* Simple function to check if input is changed from last_value by at least threshold amount*/
BOOL int16U_changed_by_threshold(INT16U input, INT16U last_value,
		INT16U threshold) {

	if (input > last_value && input - last_value > threshold)
		return TRUE;

	if (last_value > input && last_value - input > threshold)
		return TRUE;

	return FALSE;
}

