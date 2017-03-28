/*
 * util.c
 *
 * Status: T
 *
 *  Created on: Feb 3, 2017
 *      Author: Fred
 *
 *      Contains helper function implementations
 */

#include "util.h"

/* Simple function to check if input is changed from last_value by at least threshold amount*/
BOOL int32U_changed_by_threshold(INT32U input, INT32U last_value,
		INT32U threshold) {

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
BOOL int32U_differ_by_percent(INT32U src1, INT32U src2, INT32U percent,
		INT32U accuracy) {

	INT32U larger;
	INT32U smaller;

	if (src1 > src2) {
		larger = src1;
		smaller = src2;
	} else {
		larger = src2;
		smaller = src1;
	}
	if (larger == 0)
		return FALSE;
	INT32U diff = larger - smaller;
	INT32U avg = (larger + smaller) >> 1;
	if(avg == 0)
		return FALSE;
	INT32U result = (diff * accuracy) / avg;
	INT32U threshold = percent * accuracy / 100;
	if (result <= threshold)
		return FALSE;
	else
		return TRUE;
}

pwm_gen_module* get_tps_sensor_output_pwm(){
	return get_new_pwm_module(PWM_GENERATOR_TPS_OUT_AVALON_SLAVE_PERIOD_BASE, PWM_GENERATOR_TPS_OUT_AVALON_SLAVE_DUTY_BASE, PWM_GENERATOR_TPS_OUT_AVALON_SLAVE_CONTROL_BASE, TPS_OUT_PWM_PERIOD_TICKS, 0);
}

void clean_alarm(alt_alarm* alarm){
	if(alarm == NULL)
		return;
	alt_stop_alarm(alarm);
	free(alarm);
	alarm = NULL;
}
