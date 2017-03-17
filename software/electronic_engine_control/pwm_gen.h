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

#include "system.h"
#include "custom_types.h"

#define PWM_UPDATE_ALL						0

#define PWM_UPDATE_PERIOD					1

#define PWM_UPDATE_DUTY						2

#define PWM_PARAM_GET						0

#define PWM_PARAM_SET						1

#define PWM_CONTROL_ENABLE_OUTPUT			1

#define DUTY_SCALE_FACTOR					100

void set_period(pwm_gen_module* module, INT32U period);

void set_duty_cycle(pwm_gen_module* module, INT8U percent);

void enable_pwm_output(pwm_gen_module* module);

#endif /* PWM_GEN_H_ */
