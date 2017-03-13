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

#define PWM_UPDATE_ALL			0

#define PWM_UPDATE_PERIOD		1

#define PWM_UPDATE_DUTY			2

#define PWM_PARAM_GET			0

#define PWM_PARAM_SET			1

#define DUTY_SCALE_FACTOR		50

void set_period(INT16U period_us);

void set_duty_cycle(INT32U percent);

INT16U get_period();

INT8U get_duty_cycle();

#endif /* PWM_GEN_H_ */
