/*
 * pwm_gen.c
 *
 *  Created on: Mar 8, 2017
 *      Author: xuefei1
 *
 *  Status: C
 */

#include "pwm_gen.h"

static INT16U update_period(INT8U flag, INT16U period_us){
	static INT16U period = 0;
	if(PWM_PARAM_SET == flag){
		period = period_us;
	}
	return period;
}

static INT8U update_duty_cycle(INT8U flag, INT8U percent){
	static INT8U duty_cycle = 0;
	if(PWM_PARAM_SET == flag){
		duty_cycle = percent;
	}
	return duty_cycle;
}

static void update_pwm_params(){
	INT32U period = get_period();
	INT16U duty = period * get_duty_cycle() / DUTY_SCALE_FACTOR;
	*(INT16U*)PWM_GENERATOR_0_AVALON_SLAVE_DUTY_BASE = duty;
	*(INT16U*)PWM_GENERATOR_0_AVALON_SLAVE_PERIOD_BASE = get_period();
}

void set_period(INT16U period_us){
	update_period(PWM_PARAM_SET, period_us);
	update_pwm_params();
}

void set_duty_cycle(INT8U percent){
	update_duty_cycle(PWM_PARAM_SET, percent);
	update_pwm_params();
}

INT16U get_period(){
	return update_period(PWM_PARAM_GET, 0);
}

INT8U get_duty_cycle(){
	return update_duty_cycle(PWM_PARAM_GET, 0);
}
