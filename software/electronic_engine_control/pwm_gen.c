/*
 * pwm_gen.c
 *
 *  Created on: Mar 8, 2017
 *      Author: xuefei1
 *
 *  Status: C
 */

#include "pwm_gen.h"

static INT16U update_stored_period(INT8U flag, INT16U p){
	static INT16U period = 0;

	if(PWM_PARAM_SET == flag){
		period = p;
	}

	return period;
}

static INT8U update_stored_duty_cycle(INT8U flag, INT8U percent){
	static INT8U duty_cycle = 0;

	if(PWM_PARAM_SET == flag){
		duty_cycle = percent;
	}

	return duty_cycle;
}

static void update_pwm_period(){
	INT16U period = get_period();
	*(INT32U*)PWM_GENERATOR_0_AVALON_SLAVE_PERIOD_BASE = period;
}

static void update_pwm_duty(){
	INT16U duty = get_period() * get_duty_cycle() / DUTY_SCALE_FACTOR;
	*(INT32U*)PWM_GENERATOR_0_AVALON_SLAVE_DUTY_BASE = duty;
}

void set_period(INT16U period){
	update_stored_period(PWM_PARAM_SET, period);
	//update_pwm_period();
}

void set_duty_cycle(INT8U percent){
	update_stored_duty_cycle(PWM_PARAM_SET, percent);
	update_pwm_duty();
}

INT16U get_period(){
	return update_stored_period(PWM_PARAM_GET, 0);
}

INT8U get_duty_cycle(){
	return update_stored_duty_cycle(PWM_PARAM_GET, 0);
}
