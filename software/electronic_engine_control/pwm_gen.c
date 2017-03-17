/*
 * pwm_gen.c
 *
 *  Created on: Mar 8, 2017
 *      Author: xuefei1
 *
 *  Status: C
 */

#include "pwm_gen.h"

void enable_pwm_output(pwm_gen_module* module){
	*(INT8U*)module->control_base = PWM_CONTROL_ENABLE_OUTPUT;
}

void set_period(pwm_gen_module* module, INT32U p){
	module->period = p;
	*(INT32U*)module->period_base = module->period;
}

void set_duty_cycle(pwm_gen_module* module, INT8U percent){
	module->duty = (INT32U) ((INT64U)module->period * percent / DUTY_SCALE_FACTOR);
	*(INT32U*)module->duty_base = module->duty;
}

void get_new_pwm_module(pwm_gen_module* ptr, INT32U p_base, INT32U d_base, INT8U c_base, INT32U p, INT8U d_cycle){
	ptr = malloc(sizeof(pwm_gen_module));
	ptr->period_base = p_base;
	ptr->duty_base = d_base;
	ptr->control_base = c_base;
	set_period(ptr, p);
	set_duty_cycle(ptr, d_cycle);
}
