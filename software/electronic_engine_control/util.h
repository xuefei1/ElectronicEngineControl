/*
 * util.h
 *
 *  Created on: Feb 3, 2017
 *      Author: Fred
 *
 *  Contains helper function definitions
 */

#ifndef UTIL_H_
#define UTIL_H_

#include "proj_config.h"

BOOL int16U_changed_by_threshold(INT16U input, INT16U last_value, INT16U threshold);

BOOL int16U_differ_by_percent(INT16U src1, INT16U src2, INT8U percent, INT16U accuracy);

#endif /* UTIL_H_ */
