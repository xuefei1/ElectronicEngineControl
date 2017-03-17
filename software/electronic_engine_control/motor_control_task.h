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

struct motor_control_request {
   INT32U value;
   INT8U  request_type;
};

void motor_control_task(void* pdata);

//post a new request, get a queue indicating result
OSEVENT* post_new_request(motor_control_request* req);

#endif /* MOTOR_CONTROL_TASK_H_ */
