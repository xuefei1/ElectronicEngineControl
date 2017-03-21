/*
 * pwm_gen.h
 *
 *  Created on: Mar 8, 2017
 *      Author: xuefei1
 *
 *  Status: C
 */

#ifndef PWM_GEN_H_
#define PWM_GEN_H_

#include "proj_config.h"

#define PWM_CONTROL_ENABLE_OUTPUT			1

#define DUTY_SCALE_FACTOR					100

void set_period(pwm_gen_module* module, INT32U period);

void set_duty_cycle(pwm_gen_module* module, INT8U percent);

void enable_pwm_output(pwm_gen_module* module);

void disable_pwm_output(pwm_gen_module* module);

pwm_gen_module* get_new_pwm_module(INT32U p_base, INT32U d_base, INT32U c_base, INT32U p, INT8U d_cycle);

#endif /* PWM_GEN_H_ */
