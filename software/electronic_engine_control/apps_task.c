/*
 * apps_motor_task.c
 *
 * Status: E
 *
 *  Created on: Feb 2, 2017
 *      Author: Fred
 *
 *      Process handling accelerator pedal position input
 *      and digital servo motor driving
 *
 *		*See proj_config.h for acronym translations
 */
#include "apps_task.h"

/* Used to accept possible commands from other modules */
OS_EVENT *motor_cmd_q;

/* Flag indicating possible APPS failure */
OS_EVENT *apps_failure_flag;

/* Flag indicating possible motor failure */
OS_EVENT *motor_failure_flag;

/* Flag indicating failures detected in other tasks */
OS_EVENT *external_failure_flag;

/* Flag indicating failure resolved */
OS_EVENT *failure_resolved_flag;

OS_EVENT *failure_msg_q;

OS_EVENT *exit_shift_matching_flag;

alt_up_de0_nano_adc_dev* adc;

INT16U motor_cmd_q_buf[MOTOR_CMD_Q_SIZE_ELEMENTS];

//<TODO>Get rid of global if possible
INT16U *expected_tps_value;

/* Callback function after 100 ms position sensor comparison timeout */
alt_u32 apps_value_comp_callback(void* context){
	alt_up_de0_nano_adc_update(adc);
	INT16U apps_1_reading = alt_up_de0_nano_adc_read(adc,
			APPS_1_ADC_CHANNEL);
	INT16U apps_2_reading = alt_up_de0_nano_adc_read(adc,
			APPS_2_ADC_CHANNEL);

	if (APPS_VALUE_MISMATCH(apps_1_reading, apps_2_reading)) {
		OSSemPost(apps_failure_flag);
		OSQPost(failure_msg_q, (void*) ERR_APPS_READING_MISMATCH);
	}
	return 0;
}

/*  Task routine for pedal position sensor and motor */
void apps_motor_task(void* pdata) {

	adc = get_adc();
	INT8U err;
	BOOL apps_check_timer_activated = FALSE;

	failure_msg_q = get_failure_msg_q();

	apps_failure_flag = OSSemCreate(SEM_FLAG_NO_ERROR);
	motor_failure_flag = OSSemCreate(SEM_FLAG_NO_ERROR);
	external_failure_flag = OSSemCreate(SEM_FLAG_NO_ERROR);
	failure_resolved_flag = OSSemCreate(SEM_FLAG_ERROR_UNRESOLVED);
	exit_shift_matching_flag = OSSemCreate(SHIFT_MATCHING_IN_PROGRESS);

	BOOL shift_matching_mode = FALSE;
	INT32U target_RPM = 0;
	BOOL slip_control_mode = FALSE;

	static INT16U last_apps_1_reading = 0;
	static INT16U last_apps_2_reading = 0;

	motor_cmd_q = OSQCreate((void*) motor_cmd_q_buf,
			MOTOR_CMD_Q_SIZE_ELEMENTS);

	if (motor_cmd_q == NULL) {
		printf("failed to init q\n");
		return;
	}

	alt_alarm* alarm;

#if defined(RUN_AVG_TASK_TIME_TEST)
	if(alt_timestamp_start()<0)
	{
		printf("No timestamp timer\n");
		return;
	}

	INT32U iteration_count = 0;
	INT32U end_tick = 0;
	INT32U start_tick = alt_timestamp();
#endif

	//main loop
	//<TODO> Maybe change this loop from infinite later as
	//we can stop testing the APPS if an error occurs
	while (1) {
		//failure checking
#if !defined(RUN_AVG_TASK_TIME_TEST)
		if(OSSemAccept(motor_failure_flag) != SEM_FLAG_NO_ERROR){
			printf("possible motor failure, block apps_task\n");
			OSSemPend(failure_resolved_flag, Q_TIMEOUT_WAIT_FOREVER, &err);
		}else if(OSSemAccept(apps_failure_flag) != SEM_FLAG_NO_ERROR){
			printf("possible APPS failure, block apps_task\n");
			apps_check_timer_activated = FALSE;
			free(alarm);
			OSSemPend(failure_resolved_flag, Q_TIMEOUT_WAIT_FOREVER, &err);
		}else if(OSSemAccept(external_failure_flag) != SEM_FLAG_NO_ERROR){
			printf("External failure, block apps_task\n");
			OSSemPend(failure_resolved_flag, Q_TIMEOUT_WAIT_FOREVER, &err);
		}
#endif

		//WSS checking
		alt_up_de0_nano_adc_update(adc);
		if(WSS_VALUE_MISMATCH(WSS_1_ADC_CHANNEL, WSS_2_ADC_CHANNEL)){
			if(slip_control_mode == TRUE){
				continue;
			}
			slip_control_mode = TRUE;
			set_new_motor_position_by_tps(SLIP_CONTROL_THROTTLE_POS);
			continue;
		}else{
			slip_control_mode = FALSE;
		}

		//Shift matching
		alt_up_de0_nano_adc_update(adc);
		if(shift_matching_mode == TRUE){
			if(RPM_VALUE_NOT_REACHED(alt_up_de0_nano_adc_read(adc, RPM_ADC_CHANNEL), target_RPM)){
				continue;
			}else{
				signal_shift_start();
				OSSemPend(exit_shift_matching_flag, Q_TIMEOUT_WAIT_FOREVER, &err);
				shift_matching_mode = FALSE;
			}
		}

		shift_req* ptr = NULL;
		if(shift_matching_mode == FALSE){
			ptr = (shift_req*) OSQAccept(motor_cmd_q, &err);
			if(ptr != NULL){
				shift_matching_mode = TRUE;
				target_RPM = get_new_rpm_needed(alt_up_de0_nano_adc_read(adc, RPM_ADC_CHANNEL), ptr->curr_gear, ptr->new_gear);
				continue;
			}
		}

		//APPS checking
		alt_up_de0_nano_adc_update(adc);
		INT16U apps_1_reading = alt_up_de0_nano_adc_read(adc,
				APPS_1_ADC_CHANNEL);
		//Must accommodate the natural difference in readings
		INT16U apps_2_reading = alt_up_de0_nano_adc_read(adc,
				APPS_2_ADC_CHANNEL) + APPS_SENSOR_OFFSET;

		if (APPS_VALUE_CHANGED(apps_1_reading,
				last_apps_1_reading)
				|| APPS_VALUE_CHANGED(apps_2_reading, last_apps_2_reading)) {
#if !defined(RUN_AVG_TASK_TIME_TEST)
			printf("apps1 read value:%d\n", apps_1_reading);
			printf("apps2 read value:%d\n", apps_2_reading);
#endif

			if (APPS_VALUE_MISMATCH(apps_1_reading, apps_2_reading)) {
				//we have a mismatch, check again after 100 ms
				if(apps_check_timer_activated == FALSE){
					apps_check_timer_activated = TRUE;
					printf("set alarm\n");
					alarm = (alt_alarm*)malloc(sizeof(alt_alarm));
					alt_alarm_start(alarm, SENSOR_VAL_COMP_DELAY_TICKS, &apps_value_comp_callback, NULL);
				}
			} else {
				if(apps_check_timer_activated == TRUE){
					printf("clear alarm\n");
					alt_alarm_stop(alarm);
					free(alarm);
					apps_check_timer_activated = FALSE;
				}

				INT16U final_apps_value = (apps_1_reading + apps_2_reading) / 2;
				set_new_motor_position(final_apps_value);
			}
			last_apps_1_reading = apps_1_reading;
			last_apps_2_reading = apps_2_reading;
		}

#if defined(RUN_AVG_TASK_TIME_TEST)
		iteration_count++;
		if(iteration_count % AVG_ITERATION == 0){
			end_tick = alt_timestamp();
			printf("a%d:%d\n", iteration_count, (end_tick - start_tick)/iteration_count);
		}
#endif
		OSTimeDlyHMSM(APPS_MOTOR_TASK_DELAY_HOURS,
				APPS_MOTOR_TASK_DELAY_MINUTES, APPS_MOTOR_TASK_DELAY_SECONDS,
				APPS_MOTOR_TASK_DELAY_MILLISEC);
	}
}

/* Getter for motor command Q for other modules to use */
OS_EVENT* get_motor_cmd_q() {
	return motor_cmd_q;
}

/* Set motor to reach a new position, return true if no error occurred */
BOOL set_new_motor_position(INT16U apps_reading) {
	INT8U err;
	motor_control_request* req = (motor_control_request*) malloc(sizeof(motor_control_request));
	req->request_type = MOTOR_CONTROL_REQ_TPS_POS;
	req->value = get_tps_from_apps(apps_reading);
	OS_EVENT* result_q = post_new_request(req);
	INT16U result_code = (INT16U)OSQPend(result_q, Q_TIMEOUT_WAIT_FOREVER, &err);
	if(result_code == REQUEST_RESULT_FAIL){
		OSSemPost(motor_failure_flag);
		OSQPost(failure_msg_q, (void*) ERR_EXPECTED_THROTTLE_POS_MISMATCH);
	}
	free(req);
	return TRUE;
}

BOOL set_new_motor_position_by_tps(INT16U tps_reading) {
	INT8U err;
	motor_control_request* req = (motor_control_request*) malloc(sizeof(motor_control_request));
	req->request_type = MOTOR_CONTROL_REQ_TPS_POS;
	req->value = tps_reading;
	OS_EVENT* result_q = post_new_request(req);
	INT16U result_code = (INT16U)OSQPend(result_q, Q_TIMEOUT_WAIT_FOREVER, &err);
	if(result_code == REQUEST_RESULT_FAIL){
		OSSemPost(motor_failure_flag);
		OSQPost(failure_msg_q, (void*) ERR_EXPECTED_THROTTLE_POS_MISMATCH);
	}
	free(req);
	return TRUE;
}

/* The input to this function is a scaled integer from 0 to 1000 representing
 * throttle positions from 0% (fully closed) to 100.0% (fully open).
 *
 * On success, returns a scaled integer from 0 to 10000 representing duty
 * cycle from 0% to 10.000%. Returns ILLEGAL_THROTTLE_POSITION if the
 * desired position is not allowed. This function is designed to run a hobby
 * servo but will be changed to run the DC motor later.
 */
INT32U hitec_servo_demo(INT16U commanded_position) {
	if(commanded_position > THROTTLE_POSITION_MAX
			|| commanded_position < THROTTLE_POSITION_MIN) {
		return ILLEGAL_THROTTLE_POSITION;
	}

	//Hobby servos go from 0 to 180 degrees so we will use the middle 90
	//degrees for the IO demo. Hobby servos use a PWM with frequency of
	//50Hz and varying pulse widths to command different angles with
	//0.9ms being 0, 1.5ms being 90, and 2.1ms being 180.
	//http://hitecrcd.com/faqs/servos/general-servos
	//
	//The pulse_width will be a scaled integer from 0 to 2000 with 0
	//being 0ms and 2000 being 2.000ms. This is chosen as the motor
	//is likely only precise to the nearest microsecond.
	//pulse_width = commanded_position * 2 / 3 + 1200;
	//
	//The total width of a 50Hz PWM is 20ms so we'll set the duty cycle
	//so the pulse width is the desired value.
	//
	//The duty cycle will be a scaled integer from 0 to 10000 with 0
	//being a duty cycle of 0% and 10000 being a duty cycle of 10.000%.
	//This is acceptable because our duty cycle bounds are 6% and 9%
	//as we are using the middle 90degrees of the servo.
	//
	//Angle | Input | * 2 / 3 | + 1200 |  * 5  | Exact
	//[deg] |       |         |        |       | Output
	//==================================================
	//   0  |    0  |     0   |  1200  |     0 | 6
	//  10  |  100  |    67   |  1267  |  6335 | 6.33333
	//  15  |  150  |   100   |  1300  |  6500 | 6.5
	//  20  |  200  |   133   |  1333  |  6665 | 6.66667
	//  30  |  300  |   200   |  1400  |  7000 | 7
	//  40  |  400  |   267   |  1467  |  7335 | 7.33333
	//  45  |  450  |   300   |  1500  |  7500 | 7.5
	//  50  |  500  |   333   |  1533  |  7665 | 7.66667
	//  60  |  600  |   400   |  1600  |  8000 | 8
	//  70  |  700  |   467   |  1667  |  8335 | 8.33333
	//  75  |  750  |   500   |  1700  |  8500 | 8.5
	//  80  |  800  |   533   |  1733  |  8665 | 8.66667
	//  90  |  900  |   600   |  1800  |  9000 | 9

	INT32U duty_cycle = (commanded_position * 2 / 3 + 1200) * 5;

	return duty_cycle;
}

OS_EVENT* get_motor_failure_flag(){
	return motor_failure_flag;
}

OS_EVENT* get_apps_motor_task_external_failure_flag(){
	return external_failure_flag;
}

OS_EVENT* get_apps_motor_task_failure_resolved_flag(){
	return failure_resolved_flag;
}

void signal_exit_shift_matching(){
	OSSemPost(exit_shift_matching_flag);
}
