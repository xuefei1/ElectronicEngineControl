/*
 * util.h
 *
 * Status: C
 *
 *  Created on: Feb 3, 2017
 *      Author: Fred
 *
 *  Contains helper function definitions
 */

#ifndef UTIL_H_
#define UTIL_H_

#include "proj_config.h"
#include "pwm_gen.h"

BOOL int32U_changed_by_threshold(INT32U input, INT32U last_value, INT32U threshold);

BOOL int32U_differ_by_percent(INT32U src1, INT32U src2, INT32U percent, INT32U accuracy);

pwm_gen_module* get_tps_sensor_output_pwm();

void set_tps_sensor_output(pwm_gen_module* pwm, INT32U tps_val);

#endif /* UTIL_H_ */
