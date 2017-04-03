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

#define max(a,b)   ({ __typeof__ (a) _a = (a);  __typeof__ (b) _b = (b); \
 _a > _b ? _a : _b; })

#define min(a,b)   ({ __typeof__ (a) _a = (a);  __typeof__ (b) _b = (b); \
 _a > _b ? _b : _a; })

#define avg(a,b) (a+b)/2

BOOL int32U_changed_by_threshold(INT32U input, INT32U last_value, INT32U threshold);

BOOL int32U_differ_by_percent(INT32U src1, INT32U src2, INT32U percent, INT32U accuracy);

pwm_gen_module* get_tps_sensor_output_pwm();

void set_tps_sensor_output(pwm_gen_module* pwm, INT32U tps_val);

void clean_alarm(alt_alarm** alarm);

INT32U calc_percent(INT32U dividend, INT32U divisor, INT32U scale_factor);

#endif /* UTIL_H_ */
