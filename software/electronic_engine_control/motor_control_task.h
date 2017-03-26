/*
 * motor_control_task.h
 *
 *  Created on: Mar 16, 2017
 *      Author: Fred
 */

#ifndef MOTOR_CONTROL_TASK_H_
#define MOTOR_CONTROL_TASK_H_

#include "proj_config.h"
#include "util.h"
#include "pwm_gen.h"
#include "throttle_data.h"
#include "altera_avalon_pio_regs.h"

#define MOTOR_CONTROL_TASK_PRIORITY			2

/*
* Duty cycle will increase/decrease by 1 every time this value is
* reached in the task, to give throttle plate time to react to the new duty cycle
*/
#define THROTTLE_POS_CHECK_DELAY_COUNT		50

#define MOTOR_CONTROL_REQ_DEG				0
#define MOTOR_CONTROL_REQ_RPM				1

#define H_BRIDGE_IN1_HIGH					1
#define H_BRIDGE_IN2_HIGH					2
#define H_BRIDGE_ALL_LOW					0
#define H_BRIDGE_ALL_HIGH					3

#define REQUEST_RESULT_OK					0
#define REQUEST_RESULT_FAIL_TIMEOUT			1
#define REQUEST_RESULT_FAIL_TPS				2

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

/* detect if TPS expected and actual value differ */
#define TPS_VALUE_DIFFER_FROM_EXPECTED(input, exp)	int32U_changed_by_threshold(input, exp, TPS_VALUE_TOLERANCE)
/* detect if RPM expected and actual value differ */
#define RPM_VALUE_DIFFER_FROM_EXPECTED(input, exp)	int32U_changed_by_threshold(input, exp, RPM_VALUE_TOLERANCE)
/* detect if TPS readings differ by a percentage */
#define TPS_VALUE_MISMATCH(input1, input2)			int32U_changed_by_threshold(input1, input2, TPS_VALUE_DIFFERENCE)

typedef struct  {
   INT32U value;
   INT8U  request_type;
} motor_control_request;

void motor_control_task(void* pdata);

//post a new request, get a queue indicating result
OS_EVENT* post_new_request(motor_control_request* req);

#endif /* MOTOR_CONTROL_TASK_H_ */
