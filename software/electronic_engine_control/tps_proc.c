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

/*  Task routine for interfacing with throttle position sensors */
void tps_task(void* pdata) {

	INT8U err;

	alt_up_de0_nano_adc_dev* adc = get_adc();

	OS_EVENT *expected_tps_reading_q = get_expected_motor_pos_q();

	OS_EVENT *new_tps_reading_sem = get_new_tps_reading_sem();

	while (1) {

		OSSemPend(new_tps_reading_sem, SEM_TIMEOUT_WAIT_FOREVER, &err);

		if (err)
			disp_err(err, "Error pending on new reading sem");

		INT16U expected_pos = *(INT16U*) OSQPend(expected_tps_reading_q,
				Q_TIMEOUT_WAIT_FOREVER, &err);

		if (err)
			disp_err(err, "Error pending on expected value q");

		alt_up_de0_nano_adc_update(adc);

		INT16U tps_1_reading = alt_up_de0_nano_adc_read(adc, TPS_1_ADC_CHANNEL);
		INT16U tps_2_reading = alt_up_de0_nano_adc_read(adc, TPS_2_ADC_CHANNEL);

		if (TPS_VALUE_MISMATCH(tps_1_reading, tps_2_reading)) {
			//we have a failure, signal failure handler

		} else {
			//determine if throttle reached expected position
			if (TPS_VALUE_DIFFER_FROM_EXPECTED(tps_2_reading, expected_pos)
					|| TPS_VALUE_DIFFER_FROM_EXPECTED(tps_1_reading, expected_pos)) {
				//possible motor failure, signal failure handler

			}
		}
		OSTimeDlyHMSM(TPS_TASK_DELAY_HOURS, TPS_TASK_DELAY_MINUTES,
				TPS_TASK_DELAY_SECONDS, TPS_TASK_DELAY_MILLISEC);
	}
}
