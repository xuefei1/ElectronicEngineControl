/*
 * motor_control_task.c
 *
 *  Created on: Mar 16, 2017
 *      Author: Fred
 *
 *  Status: C
 */

#include "motor_control_task.h"

/* Used to queue requests */
OS_EVENT *request_q;

/* Used to queue results */
OS_EVENT *req_result_q;

INT8U request_q_buf[REQUEST_Q_SIZE_ELEMENTS];

INT8U result_q_buf[RESULT_Q_SIZE_ELEMENTS];

BOOL timeout_failure_encountered = FALSE;

BOOL tps_failure_encountered = FALSE;

alt_up_de0_nano_adc_dev* adc;

alt_alarm* watchdog;

alt_alarm* tps_check_alarm;

OS_EVENT* post_new_request(motor_control_request* req){
	OSQPost(request_q, req);
	return req_result_q;
}

void increase_throttle(pwm_gen_module* pwm_throttle_open, pwm_gen_module* pwm_throttle_close, INT8U* curr_duty_cycle){
	INT8U new_duty_cycle = *curr_duty_cycle - 1;
	if(new_duty_cycle < MOTOR_PWM_DUTY_CYCLE_FULLY_OPEN){
		new_duty_cycle = MOTOR_PWM_DUTY_CYCLE_FULLY_OPEN;
	}else if(new_duty_cycle > MOTOR_PWM_DUTY_CYCLE_FULLY_CLOSE){
		new_duty_cycle = MOTOR_PWM_DUTY_CYCLE_FULLY_CLOSE;
	}
	set_duty_cycle(pwm_throttle_open, new_duty_cycle);
	set_duty_cycle(pwm_throttle_close, PWM_DUTY_CYCLE_HIGH);
	*curr_duty_cycle = new_duty_cycle;
}

void decrease_throttle(pwm_gen_module* pwm_throttle_open, pwm_gen_module* pwm_throttle_close, INT8U* curr_duty_cycle){
	set_duty_cycle(pwm_throttle_close, PWM_DUTY_CYCLE_HIGH);
	set_duty_cycle(pwm_throttle_open, PWM_DUTY_CYCLE_HIGH);
}

alt_u32 watchdog_callback(void* context){
	free(watchdog);
	timeout_failure_encountered = TRUE;
	return 0;
}

/* Callback function to check TPS agreement position */
alt_u32 tps_value_comp_callback(void* context){
	alt_up_de0_nano_adc_update(adc);
	INT32U tps_1_reading = alt_up_de0_nano_adc_read(adc, TPS_1_ADC_CHANNEL);
	INT32U tps_2_reading = alt_up_de0_nano_adc_read(adc, TPS_2_ADC_CHANNEL);
	if (TPS_VALUE_MISMATCH(tps_1_reading, tps_2_reading)) {
		tps_failure_encountered = TRUE;
	}
	free(tps_check_alarm);
	*(BOOL*)context = FALSE;
	return 0;
}

void motor_control_task(void* pdata) {
	INT8U err;
	adc = get_adc();
	request_q = OSQCreate((void*) request_q_buf, REQUEST_Q_SIZE_ELEMENTS);
	req_result_q = OSQCreate((void*) result_q_buf, RESULT_Q_SIZE_ELEMENTS);
	BOOL serving_request = FALSE;
	BOOL tps_check_timer_activated = FALSE;
	INT16U expected_throttle_open_deg = 0;
	INT16U expected_rpm = 0;
	INT32U expected_tps_reading = 0;
	INT32U curr_tps_reading = 0;
	INT32U throttle_move_delay_counter = THROTTLE_POS_CHECK_DELAY_COUNT;
	INT8U curr_duty_cycle = MOTOR_PWM_DUTY_CYCLE_FULLY_CLOSE;
	INT8U* curr_duty_cycle_ptr = &curr_duty_cycle;
	motor_control_request* req;
	pwm_gen_module* pwm_throttle_open = get_new_pwm_module(PWM_GENERATOR_THROTTLE_OPEN_AVALON_SLAVE_PERIOD_BASE,
			PWM_GENERATOR_THROTTLE_OPEN_AVALON_SLAVE_DUTY_BASE,
			PWM_GENERATOR_THROTTLE_OPEN_AVALON_SLAVE_CONTROL_BASE,
			MOTOR_PWM_PERIOD_TICKS,
			MOTOR_PWM_DUTY_CYCLE_FULLY_CLOSE);
	pwm_gen_module* pwm_throttle_close = get_new_pwm_module(PWM_GENERATOR_THROTTLE_CLOSE_AVALON_SLAVE_PERIOD_BASE,
			PWM_GENERATOR_THROTTLE_CLOSE_AVALON_SLAVE_DUTY_BASE,
			PWM_GENERATOR_THROTTLE_CLOSE_AVALON_SLAVE_CONTROL_BASE,
			MOTOR_PWM_PERIOD_TICKS,
			PWM_DUTY_CYCLE_HIGH);
	enable_pwm_output(pwm_throttle_open);
	enable_pwm_output(pwm_throttle_close);
	pwm_gen_module* pwm_tps_out = get_tps_sensor_output_pwm();
	enable_pwm_output(pwm_tps_out);
	INT16U* result_code = 0;
	while(1){
		if(serving_request == FALSE){
			req = OSQPend(request_q, Q_TIMEOUT_WAIT_FOREVER, &err);
			watchdog = malloc(sizeof(alt_alarm));
			if(req != NULL && req->request_type == MOTOR_CONTROL_REQ_DEG){
				serving_request = TRUE;
				expected_throttle_open_deg = req->value;
				expected_tps_reading = get_tps_from_throttle_open_deg(expected_throttle_open_deg);
				//alt_alarm_start(watchdog, MOTOR_DRIVE_DELAY_TICKS, &watchdog_callback, NULL);
			}else if(req != NULL && req->request_type == MOTOR_CONTROL_REQ_RPM){
				serving_request = TRUE;
				expected_rpm = req->value;
				alt_alarm_start(watchdog, MOTOR_RPM_DRIVE_DELAY_TICKS, &watchdog_callback, NULL);
			}
		}
		if(timeout_failure_encountered == TRUE){
			serving_request = FALSE;
			*result_code = REQUEST_RESULT_FAIL_TIMEOUT;
			OSQPost(req_result_q, (void*)result_code);
			continue;
		}else if(tps_failure_encountered == TRUE){
			serving_request = FALSE;
			*result_code = REQUEST_RESULT_FAIL_TPS;
			OSQPost(req_result_q, (void*)result_code);
			continue;
		}
		alt_up_de0_nano_adc_update(adc);
		if(req != NULL && req->request_type == MOTOR_CONTROL_REQ_DEG){
			INT16U tps_1_reading = alt_up_de0_nano_adc_read(adc, TPS_1_ADC_CHANNEL);
			INT16U tps_2_reading = alt_up_de0_nano_adc_read(adc, TPS_2_ADC_CHANNEL);
			if (TPS_VALUE_MISMATCH(tps_1_reading, tps_2_reading)) {
				if(tps_check_timer_activated == FALSE){
					tps_check_alarm = (alt_alarm*) malloc(sizeof(alt_alarm));
					alt_alarm_start(tps_check_alarm, SENSOR_VAL_COMP_DELAY_TICKS, &tps_value_comp_callback, &tps_check_timer_activated);
				}
			}
			curr_tps_reading = tps_1_reading;
			set_tps_sensor_output(pwm_tps_out, curr_tps_reading);
			if(FALSE == TPS_VALUE_DIFFER_FROM_EXPECTED(curr_tps_reading, expected_tps_reading)){
				alt_alarm_stop(watchdog);
				serving_request = FALSE;
				*result_code = REQUEST_RESULT_OK;
				*curr_duty_cycle_ptr = MOTOR_PWM_DUTY_CYCLE_FULLY_CLOSE;
				OSQPost(req_result_q, (void*)result_code);
			}else if(curr_tps_reading > expected_tps_reading){
				if(throttle_move_delay_counter == THROTTLE_POS_CHECK_DELAY_COUNT){
					decrease_throttle(pwm_throttle_open, pwm_throttle_close, curr_duty_cycle_ptr);;
					throttle_move_delay_counter = 0;
				}else{
					throttle_move_delay_counter++;
				}
			}else{
				if(throttle_move_delay_counter == THROTTLE_POS_CHECK_DELAY_COUNT){
					increase_throttle(pwm_throttle_open, pwm_throttle_close, curr_duty_cycle_ptr);
					throttle_move_delay_counter = 0;
				}else{
					throttle_move_delay_counter++;
				}
			}
		}else if(req != NULL && req->request_type == MOTOR_CONTROL_REQ_RPM){
			INT16U rpm_reading = alt_up_de0_nano_adc_read(adc, RPM_ADC_CHANNEL);
			if(FALSE == RPM_VALUE_DIFFER_FROM_EXPECTED(rpm_reading, expected_rpm)){
				alt_alarm_stop(watchdog);
				serving_request = FALSE;
				*result_code = REQUEST_RESULT_OK;
				*curr_duty_cycle_ptr = MOTOR_PWM_DUTY_CYCLE_FULLY_CLOSE;
				OSQPost(req_result_q, (void*)result_code);
			}else if(rpm_reading > expected_rpm){
				if(throttle_move_delay_counter == THROTTLE_POS_CHECK_DELAY_COUNT){
					decrease_throttle(pwm_throttle_open, pwm_throttle_close, curr_duty_cycle_ptr);;
					throttle_move_delay_counter = 0;
				}else{
					throttle_move_delay_counter++;
				}
			}else{
				if(throttle_move_delay_counter == THROTTLE_POS_CHECK_DELAY_COUNT){
					increase_throttle(pwm_throttle_open, pwm_throttle_close, curr_duty_cycle_ptr);
					throttle_move_delay_counter = 0;
				}else{
					throttle_move_delay_counter++;
				}
			}
		}

		OSTimeDlyHMSM(MOTOR_CONTROL_TASK_DELAY_HOURS,
				MOTOR_CONTROL_TASK_DELAY_MINUTES, MOTOR_CONTROL_TASK_DELAY_SECONDS,
				MOTOR_CONTROL_TASK_DELAY_MILLISEC);
	}
}
