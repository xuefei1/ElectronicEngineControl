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

	alt_up_de0_nano_adc_dev* adc = alt_up_de0_nano_adc_open_dev(
			DE0_NANO_ADC_0_NAME);

	static INT16U last_apps_1_reading = 0;
	static INT16U last_apps_2_reading = 0;
	while (1) {
		alt_up_de0_nano_adc_update(adc);

		INT16U apps_1_reading = alt_up_de0_nano_adc_read(adc,
				APPS_1_ADC_CHANNEL);
		INT16U apps_2_reading = alt_up_de0_nano_adc_read(adc,
				APPS_2_ADC_CHANNEL);

		if (APPS_VALUE_CHANGED(apps_1_reading,
				last_apps_1_reading) || APPS_VALUE_CHANGED(apps_2_reading, last_apps_2_reading)) {
				if(APPS_VALUE_MISMATCH(apps_1_reading, apps_2_reading)){
					//we have a failure, turn off motor, indicate failure and block

				}else{
					//determine new motor position

				}
		}
		OSTimeDlyHMSM(APPS_MOTOR_TASK_DELAY_HOURS,
		APPS_MOTOR_TASK_DELAY_MINUTES, APPS_MOTOR_TASK_DELAY_SECONDS,
		APPS_MOTOR_TASK_DELAY_MILLISEC);
	}
}

