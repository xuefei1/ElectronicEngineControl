/*
 * tps_proc.c
 *
 * Status: C
 *
 *  Created on: Feb 4, 2017
 *      Author: xuefei1
 *
 *      Throttle Position Sensor process
 *
 *      *See proj_config.h for acronym translations
 */

#include "tps_proc.h"
#include "apps_motor_proc.h"

/* Flag indicating possible APPS failure */
OS_EVENT *tps_failure_flag;

/* Flag indicating possible APPS failure */
OS_EVENT *motor_failure_flag;

/* Flag indicating failures detected in other tasks */
OS_EVENT *external_failure_flag;

/* Flag indicating failure resolved */
OS_EVENT *failure_resolved_flag;

OS_EVENT* failure_msg_q;

INT16U curr_expected_pos;

alt_up_de0_nano_adc_dev* adc;

alt_u32 motor_pos_check_callback(void* context);

alt_u32 tps_value_comp_callback(void* context);

/*  Task routine for interfacing with throttle position sensors */
void tps_task(void* pdata) {

	INT8U err;

	adc = get_adc();

	OS_EVENT *expected_tps_reading_q = get_expected_motor_pos_q();

	failure_msg_q = get_failure_msg_q();

	tps_failure_flag = OSSemCreate(SEM_FLAG_NO_ERROR);

	motor_failure_flag = OSSemCreate(SEM_FLAG_NO_ERROR);

	external_failure_flag = OSSemCreate(SEM_FLAG_NO_ERROR);

	failure_resolved_flag = OSSemCreate(SEM_FLAG_ERROR_UNRESOLVED);

	alt_alarm* expected_pos_alarm;

	BOOL expected_pos_check_timer_activated = FALSE;

	alt_alarm* tps_check_alarm;

	BOOL tps_check_timer_activated = FALSE;

	while (1) {
		//failure checking
		if(OSSemAccept(motor_failure_flag) != SEM_FLAG_NO_ERROR){
			printf("possible motor failure, block tps_task\n");
			expected_pos_check_timer_activated = FALSE;
			free(expected_pos_alarm);
			if(tps_check_timer_activated == TRUE){
				tps_check_timer_activated = FALSE;
				alt_alarm_stop (tps_check_alarm);
				free(tps_check_alarm);
			}
			OSSemPend(failure_resolved_flag, Q_TIMEOUT_WAIT_FOREVER, &err);
		}else if(OSSemAccept(tps_failure_flag) != SEM_FLAG_NO_ERROR){
			printf("possible tps failure, block tps_task\n");
			tps_check_timer_activated = FALSE;
			free(tps_check_alarm);
			if(expected_pos_check_timer_activated == TRUE){
				expected_pos_check_timer_activated = FALSE;
				alt_alarm_stop (expected_pos_alarm);
				free(expected_pos_alarm);
			}
			OSSemPend(failure_resolved_flag, Q_TIMEOUT_WAIT_FOREVER, &err);
		}else if(OSSemAccept(external_failure_flag) != SEM_FLAG_NO_ERROR){
			printf("External failure, block apps_motor_task\n");
			OSSemPend(failure_resolved_flag, Q_TIMEOUT_WAIT_FOREVER, &err);
		}

		INT16U expected_pos = *(INT16U*) OSQPend(expected_tps_reading_q,
				Q_TIMEOUT_WAIT_FOREVER, &err);
		if (err)
			disp_err(err, "Error pending on expected value q");

		//failure checking
		if(OSSemAccept(motor_failure_flag) != SEM_FLAG_NO_ERROR){
			printf("possible motor failure, block tps_task\n");
			expected_pos_check_timer_activated = FALSE;
			free(expected_pos_alarm);
			if(tps_check_timer_activated == TRUE){
				tps_check_timer_activated = FALSE;
				alt_alarm_stop (tps_check_alarm);
				free(tps_check_alarm);
			}
			OSSemPend(failure_resolved_flag, Q_TIMEOUT_WAIT_FOREVER, &err);
		}else if(OSSemAccept(tps_failure_flag) != SEM_FLAG_NO_ERROR){
			printf("possible tps failure, block tps_task\n");
			tps_check_timer_activated = FALSE;
			free(tps_check_alarm);
			if(expected_pos_check_timer_activated == TRUE){
				expected_pos_check_timer_activated = FALSE;
				alt_alarm_stop (expected_pos_alarm);
				free(expected_pos_alarm);
			}
			OSSemPend(failure_resolved_flag, Q_TIMEOUT_WAIT_FOREVER, &err);
		}else if(OSSemAccept(external_failure_flag) != SEM_FLAG_NO_ERROR){
			printf("External failure, block apps_motor_task\n");
			OSSemPend(failure_resolved_flag, Q_TIMEOUT_WAIT_FOREVER, &err);
		}

		curr_expected_pos = expected_pos;

		//activate expected position checker
		if(expected_pos_check_timer_activated == TRUE){
			alt_alarm_stop (expected_pos_alarm);
			free(expected_pos_alarm);
		}
		expected_pos_alarm = (alt_alarm*) malloc(sizeof(alt_alarm));
		alt_alarm_start(expected_pos_alarm, MOTOR_DRIVE_DELAY_TICKS, &motor_pos_check_callback, NULL);

		alt_up_de0_nano_adc_update(adc);
		INT16U tps_1_reading = alt_up_de0_nano_adc_read(adc, TPS_1_ADC_CHANNEL);
		INT16U tps_2_reading = alt_up_de0_nano_adc_read(adc, TPS_2_ADC_CHANNEL);
		if (TPS_VALUE_MISMATCH(tps_1_reading, tps_2_reading)) {
			if(tps_check_timer_activated == FALSE){
				tps_check_alarm = (alt_alarm*) malloc(sizeof(alt_alarm));
				alt_alarm_start(tps_check_alarm, SENSOR_VAL_COMP_DELAY_TICKS, &tps_value_comp_callback, NULL);
			}
		}else{
			if(tps_check_timer_activated == TRUE){
				tps_check_timer_activated = FALSE;
				alt_alarm_stop (tps_check_alarm);
				free(tps_check_alarm);
			}
		}
		OSTimeDlyHMSM(TPS_TASK_DELAY_HOURS, TPS_TASK_DELAY_MINUTES,
				TPS_TASK_DELAY_SECONDS, TPS_TASK_DELAY_MILLISEC);
	}
}

/* Callback function to check motor position */
alt_u32 motor_pos_check_callback(void* context){
	alt_up_de0_nano_adc_update(adc);
	INT16U tps_1_reading = alt_up_de0_nano_adc_read(adc, TPS_1_ADC_CHANNEL);
	INT16U tps_2_reading = alt_up_de0_nano_adc_read(adc, TPS_2_ADC_CHANNEL);
	if (TPS_VALUE_DIFFER_FROM_EXPECTED(tps_2_reading, curr_expected_pos)
			|| TPS_VALUE_DIFFER_FROM_EXPECTED(tps_1_reading, curr_expected_pos)) {
		OSSemPost(motor_failure_flag);
		OSQPost(failure_msg_q, (void*) ERR_EXPECTED_THROTTLE_POS_MISMATCH);
	}
	return 0;
}

/* Callback function to check TPS agreement position */
alt_u32 tps_value_comp_callback(void* context){
	alt_up_de0_nano_adc_update(adc);
	INT16U tps_1_reading = alt_up_de0_nano_adc_read(adc, TPS_1_ADC_CHANNEL);
	INT16U tps_2_reading = alt_up_de0_nano_adc_read(adc, TPS_2_ADC_CHANNEL);
	if (TPS_VALUE_MISMATCH(tps_1_reading, tps_2_reading)) {
		OSSemPost(tps_failure_flag);
		OSQPost(failure_msg_q, (void*) ERR_TPS_READING_MISMATCH);
	}
	return 0;
}
