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
	module->control = PWM_CONTROL_ENABLE_OUTPUT;
	*(INT8U*)module->control_base = module->control;
}

void disable_pwm_output(pwm_gen_module* module){
	module->control = 0;
	*(INT8U*)module->control_base = module->control;
}

void set_period(pwm_gen_module* module, INT32U p){
	if(p != module->period){
		module->period = p;
		*(INT32U*)module->period_base = module->period;
	}
}

void set_duty_cycle(pwm_gen_module* module, INT8U percent){
	if(percent > PWM_DUTY_CYCLE_HIGH)
		percent = PWM_DUTY_CYCLE_HIGH;
	if(percent != module->duty_cycle){
		INT32U new_duty = (INT32U) (module->period * percent / DUTY_SCALE_FACTOR);
		module->duty_count = new_duty;
		module->duty_cycle = percent;
		*(INT32U*)module->duty_base = module->duty_count;
	}
}

void set_duty_cycle_1_sig_fig(pwm_gen_module* module, INT16U percent){
	if(percent / DUTY_SCALE_FACTOR > PWM_DUTY_CYCLE_HIGH)
		percent = PWM_DUTY_CYCLE_HIGH;
	if(percent != module->duty_cycle){
		INT32U new_duty = (INT32U) (module->period * percent / DUTY_SCALE_FACTOR_1_SIG_FIG);
		module->duty_count = new_duty;
		module->duty_cycle = percent;
		*(INT32U*)module->duty_base = module->duty_count;
	}
}

pwm_gen_module* get_new_pwm_module(INT32U p_base, INT32U d_base, INT32U c_base, INT32U p, INT8U d_cycle){
	pwm_gen_module* ptr = (pwm_gen_module*) malloc(sizeof(pwm_gen_module));
	ptr->period_base = p_base;
	ptr->duty_base = d_base;
	ptr->control_base = c_base;
	disable_pwm_output(ptr);
	set_period(ptr, p);
	set_duty_cycle(ptr, d_cycle);
	return ptr;
}
