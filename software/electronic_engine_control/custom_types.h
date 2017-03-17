/*
 * custom_types.h
 *
 *  Created on: Mar 7, 2017
 *      Author: xuefei1
 */

#ifndef CUSTOM_TYPES_H_
#define CUSTOM_TYPES_H_

#include "includes.h"

typedef INT8U BOOL;
#define TRUE  0xFF
#define FALSE 0x00

struct pwm_gen_module {
   INT32U period_base;
   INT32U duty_base;
   INT8U  control_base;
   INT32U period;
   INT32U duty;
};

#endif /* CUSTOM_TYPES_H_ */
