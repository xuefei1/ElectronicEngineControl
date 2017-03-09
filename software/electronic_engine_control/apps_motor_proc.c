/*
 * apps_motor_proc.c
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
#include "apps_motor_proc.h"
#include "altera_avalon_timer.h"

/* Used to store new expected throttle position */
OS_EVENT *expected_tps_reading_q;

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

alt_up_de0_nano_adc_dev* adc;

INT16U expected_tps_reading_q_buf[EXPECTED_TPS_READING_Q_SIZE_ELEMENTS];

INT16U motor_cmd_q_buf[MOTOR_CMD_Q_SIZE_ELEMENTS];

alt_u32 motor_pos_check_callback(void* context);

INT16U *expected_tps_value;

/*  Task routine for pedal position sensor and motor */
void apps_motor_task(void* pdata) {

	adc = get_adc();

	INT8U err;

	BOOL apps_check_timer_activated = FALSE;

	alt_alarm* expected_pos_alarm;

	BOOL expected_pos_check_timer_activated = FALSE;

	failure_msg_q = get_failure_msg_q();

	apps_failure_flag = OSSemCreate(SEM_FLAG_NO_ERROR);

	motor_failure_flag = OSSemCreate(SEM_FLAG_NO_ERROR);

	external_failure_flag = OSSemCreate(SEM_FLAG_NO_ERROR);

	failure_resolved_flag = OSSemCreate(SEM_FLAG_ERROR_UNRESOLVED);

	static INT16U last_apps_1_reading = 0;
	static INT16U last_apps_2_reading = 0;

	expected_tps_reading_q = OSQCreate((void*) expected_tps_reading_q_buf,
			EXPECTED_TPS_READING_Q_SIZE_ELEMENTS);
	if (expected_tps_reading_q == NULL) {
		printf("failed to init q\n");
		return;
	}

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
	while (1) {
		//failure checking
#if !defined(RUN_AVG_TASK_TIME_TEST)
		if(OSSemAccept(motor_failure_flag) != SEM_FLAG_NO_ERROR){
			printf("possible motor failure, block tps_task\n");
			expected_pos_check_timer_activated = FALSE;
			free(expected_pos_alarm);
			OSSemPend(failure_resolved_flag, Q_TIMEOUT_WAIT_FOREVER, &err);
		}else if(OSSemAccept(apps_failure_flag) != SEM_FLAG_NO_ERROR){
			printf("possible APPS failure, block apps_motor_task\n");
			apps_check_timer_activated = FALSE;
			free(alarm);
			OSSemPend(failure_resolved_flag, Q_TIMEOUT_WAIT_FOREVER, &err);
		}else if(OSSemAccept(external_failure_flag) != SEM_FLAG_NO_ERROR){
			printf("External failure, block apps_motor_task\n");
			OSSemPend(failure_resolved_flag, Q_TIMEOUT_WAIT_FOREVER, &err);
		}
#endif
		//WSS checking

		//Shift matching

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
				//activate expected position checker
				if(expected_pos_check_timer_activated == TRUE){
					alt_alarm_stop(expected_pos_alarm);
					free(expected_pos_alarm);
				}
				expected_pos_alarm = (alt_alarm*) malloc(sizeof(alt_alarm));
				alt_alarm_start(expected_pos_alarm, MOTOR_DRIVE_DELAY_TICKS, &motor_pos_check_callback, NULL);
				*expected_tps_value = get_expected_tps_reading(
						final_apps_value);
				OSQPost(expected_tps_reading_q, (void*) expected_tps_value);

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

/* Getter for expected motor position Q for TPS process to use */
OS_EVENT* get_expected_motor_pos_q() {
	return expected_tps_reading_q;
}

/* Getter for motor command Q for other modules to use */
OS_EVENT* get_motor_cmd_q() {
	return motor_cmd_q;
}

/* Takes in APPS reading and returns the expected TPS reading */
INT16U get_expected_tps_reading(INT16U apps_reading) {
	//Not implemented yet, but in the future
	//expected TPS reading will be either computed at run time
	//or acquired from a lookup table

	return apps_reading;
}

/* Set motor to reach a new position, return true if no error occurred */
BOOL set_new_motor_position(INT16U apps_reading) {
	OSQPost(expected_tps_reading_q, (void*) apps_reading);
	return TRUE;
}

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

/* Callback function to check motor position */
alt_u32 motor_pos_check_callback(void* context){
	OSQPost(expected_tps_reading_q, (void*) expected_tps_value);
	return 0;
}

OS_EVENT* get_motor_failure_flag(){
	return motor_failure_flag;
}
