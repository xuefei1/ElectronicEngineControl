/*
 * motor_control_task.c
 *
 *  Created on: Mar 16, 2017
 *      Author: Fred
 */

#include "motor_control_task.h"

/* Used to queue requests */
OS_EVENT *request_q;

/* Used to queue results */
OS_EVENT *req_result_q;

INT8U request_q_buf[REQUEST_Q_SIZE_ELEMENTS];

INT8U result_q_buf[RESULT_Q_SIZE_ELEMENTS];

BOOL failure_encountered = FALSE;

alt_up_de0_nano_adc_dev* adc;

OSEVENT* post_new_request(motor_control_request* req){
	OSQPost(request_q, req);
	return request_q;
}

void turn_motor_cw(pwm_gen_module* pwm){
	set_duty_cycle(pwm, MOTOR_PWM_DUTY_CYCLE_CW);
}

void turn_motor_ccw(pwm_gen_module* pwm){
	set_duty_cycle(pwm, MOTOR_PWM_DUTY_CYCLE_CCW);
}

void stop_motor(pwm_gen_module* pwm){
	set_duty_cycle(pwm, MOTOR_PWM_DUTY_CYCLE_IDLE);
}

void motor_control_task(void* pdata) {
	INT8U err;
	adc = get_adc();
	request_q = OSQCreate((void*) request_q_buf, REQUEST_Q_SIZE_ELEMENTS);
	result_q = OSQCreate((void*) result_q_buf, RESULT_Q_SIZE_ELEMENTS);
	alt_alarm* watchdog;
	BOOL serving_request = FALSE;
	INT16U expected_pos = 0;
	INT16U expected_rpm = 0;
	motor_control_request* req;
	pwm_gen_module* pwm;
	get_new_pwm_module(pwm, p_base, d_base, c_base, MOTOR_PWM_PERIOD_TICKS, MOTOR_PWM_DUTY_CYCLE_IDLE);

	enable_pwm_output(pwm);
	while(1){
		if(serving_request == FALSE){
			req = OSQPend(request_q, Q_TIMEOUT_WAIT_FOREVER, &err);
			if(req != NULL && req->request_type == MOTOR_CONTROL_REQ_TPS_POS){
				serving_request = TRUE;
				expected_pos = req->value;
			}else if(req != NULL && req->request_type == MOTOR_CONTROL_REQ_RPM){
				serving_request = TRUE;
				expected_rpm = req->value;
			}
		}
		if(failure_encountered == TRUE){
			serving_request = FALSE;
			OSQPost(req_result_q, REQUEST_RESULT_FAIL);
		}

		if(req != NULL && req->request_type == MOTOR_CONTROL_REQ_TPS_POS){

		}else if(req != NULL && req->request_type == MOTOR_CONTROL_REQ_RPM){

		}

		OSTimeDlyHMSM(MOTOR_CONTROL_TASK_DELAY_HOURS,
				MOTOR_CONTROL_TASK_DELAY_MINUTES, MOTOR_CONTROL_TASK_DELAY_SECONDS,
				MOTOR_CONTROL_TASK_DELAY_MILLISEC);
	}
}
