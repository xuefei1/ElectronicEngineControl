/*
 * apps_motor_proc.c
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
#include "altera_up_avalon_de0_nano_adc.h"

/*  Task routine for pedal position sensor and motor */
void apps_motor_task(void* pdata) {
	alt_up_de0_nano_adc_dev* adc_controller = alt_up_de0_nano_adc_open_dev(
			DE0_NANO_ADC_0_NAME);
	INT8U apps_1_channel = APPS_1_ADC_CHANNEL;
	INT8U apps_2_channel = APPS_2_ADC_CHANNEL;
	static INT16U last_apps_1_reading = 0;
	static INT16U last_apps_2_reading = 0;
	while (1) {

		OSTimeDlyHMSM(APPS_MOTOR_TASK_DELAY_HOURS,
				APPS_MOTOR_TASK_DELAY_MINUTES, APPS_MOTOR_TASK_DELAY_SECONDS,
				APPS_MOTOR_TASK_DELAY_MILLISEC);
	}
}

