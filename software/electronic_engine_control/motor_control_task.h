/*
 * motor_control_task.h
 *
 *  Created on: Mar 16, 2017
 *      Author: Fred
 */

#ifndef MOTOR_CONTROL_TASK_H_
#define MOTOR_CONTROL_TASK_H_

#include "proj_config.h"
#include "pwm_gen.h"

#define MOTOR_CONTROL_TASK_PRIORITY			1

#define MOTOR_CONTROL_REQ_TPS_POS			0
#define MOTOR_CONTROL_REQ_RPM				1

#define REQUEST_RESULT_OK					0
#define REQUEST_RESULT_FAIL					1

#define REQUEST_Q_SIZE_ELEMENTS				256
#define RESULT_Q_SIZE_ELEMENTS				256

#define MOTOR_CONTROL_TASK_DELAY_HOURS  	0
#define MOTOR_CONTROL_TASK_DELAY_MINUTES	0
#define MOTOR_CONTROL_TASK_DELAY_SECONDS	0
#define MOTOR_CONTROL_TASK_DELAY_MILLISEC	0

/* ADC channel assignments */
#define TPS_1_ADC_CHANNEL			  		2
#define TPS_2_ADC_CHANNEL			  		3
#define RPM_ADC_CHANNEL			  			6

/* detect if TPS expected and actual value differ*/
#define TPS_VALUE_DIFFER_FROM_EXPECTED(input, exp)	int32U_changed_by_threshold(input, exp, TPS_VALUE_TOLERANCE)

#define RPM_VALUE_DIFFER_FROM_EXPECTED(input, exp)	int32U_changed_by_threshold(input, exp, RPM_VALUE_TOLERANCE)

/* detect if TPS readings differ by a percentage */
#define TPS_VALUE_MISMATCH(input1, input2)			int32U_differ_by_percent(input1, input2, TPS_VALUE_DIFFERENCE_PERCENT, PERCENT_DIFF_ACCURACY)

typedef struct  {
   INT32U value;
   INT8U  request_type;
} motor_control_request;

void motor_control_task(void* pdata);

//post a new request, get a queue indicating result
OS_EVENT* post_new_request(motor_control_request* req);

#endif /* MOTOR_CONTROL_TASK_H_ */
