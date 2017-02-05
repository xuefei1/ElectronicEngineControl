/*
 * apps_motor_proc.c
 *
 * Status: C
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

/* Used to store new expected throttle position */
OS_EVENT *expected_tps_reading_q;

/* Used to signal TPS a new reading should be available */
OS_EVENT *new_tps_reading_available;

/* Used to accept possible commands from other modules */
OS_EVENT *motor_cmd_q;

char expected_tps_reading_q_buf[EXPECTED_TPS_READING_Q_SIZE_BYTE];

char motor_cmd_q_buf[MOTOR_CMD_Q_SIZE_BYTE];

/*  Task routine for pedal position sensor and motor */
void apps_motor_task(void* pdata) {

	alt_up_de0_nano_adc_dev* adc = get_adc();

	static INT16U last_apps_1_reading = 0;
	static INT16U last_apps_2_reading = 0;

	expected_tps_reading_q = OSQCreate((void*) expected_tps_reading_q_buf,
			EXPECTED_TPS_READING_Q_SIZE_BYTE / sizeof(INT16U));
	if (expected_tps_reading_q == NULL) {
		printf("failed to init q\n");
		return;
	}

	motor_cmd_q = OSQCreate((void*) motor_cmd_q_buf,
			MOTOR_CMD_Q_SIZE_BYTE / sizeof(INT16U));
	if (motor_cmd_q == NULL) {
		printf("failed to init q\n");
		return;
	}

	new_tps_reading_available = OSSemCreate(NEW_TPS_READING_SEM_COUNT);

	while (1) {
		alt_up_de0_nano_adc_update(adc);

		INT16U apps_1_reading = alt_up_de0_nano_adc_read(adc,
				APPS_1_ADC_CHANNEL);
		INT16U apps_2_reading = alt_up_de0_nano_adc_read(adc,
				APPS_2_ADC_CHANNEL);

		if (APPS_VALUE_CHANGED(apps_1_reading,
				last_apps_1_reading)
				|| APPS_VALUE_CHANGED(apps_2_reading, last_apps_2_reading)) {
			if (APPS_VALUE_MISMATCH(apps_1_reading, apps_2_reading)) {
				//we have a failure, turn off motor, indicate failure and block

			} else {
				INT16U final_apps_value = (apps_1_reading + apps_2_reading) / 2;
				set_new_motor_position(final_apps_value);
				INT16U expected_tps_value = get_expected_tps_reading(
						final_apps_value);
				OSQPost(expected_tps_reading_q, (void*) expected_tps_value);
				//Set timer interrupt for 100ms

			}
		}
		OSTimeDlyHMSM(APPS_MOTOR_TASK_DELAY_HOURS,
				APPS_MOTOR_TASK_DELAY_MINUTES, APPS_MOTOR_TASK_DELAY_SECONDS,
				APPS_MOTOR_TASK_DELAY_MILLISEC);
	}
}

/* Getter for expected motor position Q for TPS process to use */
OS_EVENT* get_expected_motor_pos_q() {
	return expected_tps_reading_q;
}

/* Getter for new TPS reading semaphore for TPS process to pend on */
OS_EVENT* get_new_tps_reading_sem() {
	return new_tps_reading_available;
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
	//Not implemented yet
	return TRUE;
}
