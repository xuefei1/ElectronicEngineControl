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

#define DUTY_SCALE_FACTOR					1000

#define PWM_DUTY_CYCLE_LOW					0
#define PWM_DUTY_CYCLE_HALF					500
#define PWM_DUTY_CYCLE_HIGH					1000

typedef struct  {
   INT32U period_base;
   INT32U duty_base;
   INT32U control_base;
   INT32U period;
   INT32U duty_count;
   INT16U duty_cycle;
   INT8U  control;
   INT8U  reserved;
} pwm_gen_module;

void set_period(pwm_gen_module* module, INT32U period);

void set_duty_cycle(pwm_gen_module* module, INT16U percent);

void enable_pwm_output(pwm_gen_module* module);

void disable_pwm_output(pwm_gen_module* module);

pwm_gen_module* get_new_pwm_module(INT32U p_base, INT32U d_base, INT32U c_base, INT32U p, INT16U d_cycle);

#endif /* PWM_GEN_H_ */
