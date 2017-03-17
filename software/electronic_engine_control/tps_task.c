/*
 * tps_task.c
 *
 * Status: E
 *
 *  Created on: Feb 4, 2017
 *      Author: xuefei1
 *
 *      Obsolete, to be removed
 */

#include "tps_task.h"
#include "apps_motor_task.h"

/* Flag indicating possible APPS failure */
OS_EVENT *tps_failure_flag;

/* Flag indicating failures detected in other tasks */
OS_EVENT *external_failure_flag;

/* Flag indicating failure resolved */
OS_EVENT *failure_resolved_flag;

OS_EVENT* failure_msg_q;

INT16U curr_expected_pos;

alt_up_de0_nano_adc_dev* adc;

alt_u32 tps_value_comp_callback(void* context);

/*  Task routine for interfacing with throttle position sensors */
void tps_task(void* pdata) {

	INT8U err;

	adc = get_adc();

	OS_EVENT *expected_tps_reading_q = get_expected_motor_pos_q();

	failure_msg_q = get_failure_msg_q();

	tps_failure_flag = OSSemCreate(SEM_FLAG_NO_ERROR);

	external_failure_flag = OSSemCreate(SEM_FLAG_NO_ERROR);

	failure_resolved_flag = OSSemCreate(SEM_FLAG_ERROR_UNRESOLVED);

	alt_alarm* tps_check_alarm;

	BOOL tps_check_timer_activated = FALSE;

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

	while (1) {

#if defined(RUN_AVG_TASK_TIME_TEST)
		INT16U expected_pos = 0;
#else
		//failure checking
		if(OSSemAccept(tps_failure_flag) != SEM_FLAG_NO_ERROR){
			printf("possible tps failure, block tps_task\n");
			tps_check_timer_activated = FALSE;
			free(tps_check_alarm);
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
		if(OSSemAccept(tps_failure_flag) != SEM_FLAG_NO_ERROR){
			printf("possible tps failure, block tps_task\n");
			tps_check_timer_activated = FALSE;
			free(tps_check_alarm);
			OSSemPend(failure_resolved_flag, Q_TIMEOUT_WAIT_FOREVER, &err);
		}else if(OSSemAccept(external_failure_flag) != SEM_FLAG_NO_ERROR){
			printf("External failure, block apps_motor_task\n");
			OSSemPend(failure_resolved_flag, Q_TIMEOUT_WAIT_FOREVER, &err);
		}
#endif

		curr_expected_pos = expected_pos;

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
			alt_up_de0_nano_adc_update(adc);
			INT16U tps_1_reading = alt_up_de0_nano_adc_read(adc, TPS_1_ADC_CHANNEL);
			INT16U tps_2_reading = alt_up_de0_nano_adc_read(adc, TPS_2_ADC_CHANNEL);
			if (TPS_VALUE_DIFFER_FROM_EXPECTED(tps_2_reading, curr_expected_pos)
					|| TPS_VALUE_DIFFER_FROM_EXPECTED(tps_1_reading, curr_expected_pos)) {
				OSSemPost(get_motor_failure_flag());
				OSQPost(failure_msg_q, (void*) ERR_EXPECTED_THROTTLE_POS_MISMATCH);
			}
		}
#if defined(RUN_AVG_TASK_TIME_TEST)
		iteration_count++;
		if(iteration_count % AVG_ITERATION == 0){
			end_tick = alt_timestamp();
			printf("t%d:%d\n", iteration_count, (end_tick - start_tick)/iteration_count);
		}
#endif
		OSTimeDlyHMSM(TPS_TASK_DELAY_HOURS, TPS_TASK_DELAY_MINUTES,
				TPS_TASK_DELAY_SECONDS, TPS_TASK_DELAY_MILLISEC);
	}
}

OS_EVENT* get_tps_task_failure_resolved_flag(){
	return failure_resolved_flag;
}

OS_EVENT* get_tps_task_external_failure_flag(){
	return external_failure_flag;
}
