/*
 * apps_motor_proc.h
 *
 * Status: C
 *
 *  Created on: Feb 2, 2017
 *      Author: Fred
 *
 *      Interface for apps_motor_proc.c
 *
 *      *See proj_config.h for acronym translations
 */

#ifndef APPS_MOTOR_PROC_H_
#define APPS_MOTOR_PROC_H_

#include "proj_config.h"
#include "util.h"

#define APPS_MOTOR_TASK_PRIORITY      		2

/* ADC channel assignments */
#define APPS_1_ADC_CHANNEL			  		0
#define APPS_2_ADC_CHANNEL			  		1

/* Task Delays*/
#define APPS_MOTOR_TASK_DELAY_HOURS	  		0
#define APPS_MOTOR_TASK_DELAY_MINUTES	  	0
#define APPS_MOTOR_TASK_DELAY_SECONDS	  	0
#define APPS_MOTOR_TASK_DELAY_MILLISEC	  	50

/* detect if there is a new APPS value*/
#define APPS_VALUE_CHANGED(input, last_value)	int16U_changed_by_threshold(input, last_value, APPS_VALUE_CHANGE_THRESHOLD)

/* detect if APPS reading differs by a percentage */
#define APPS_VALUE_MISMATCH(input1, input2)		int16U_differ_by_percent(input1, input2, APPS_VALUE_DIFFERENCE_PERCENT, PERCENT_DIFF_ACCURACY)

#define EXPECTED_TPS_READING_Q_SIZE_BYTE	256

#define MOTOR_CMD_Q_SIZE_BYTE				64

#define NEW_TPS_READING_SEM_COUNT			0

void apps_motor_task(void* pdata);

OS_EVENT* get_expected_motor_pos_q();

OS_EVENT* get_motor_cmd_q();

INT16U get_expected_tps_reading(INT16U apps_reading);

BOOL set_new_motor_position(INT16U apps_reading);

alt_u32 apps_value_comp_callback(void* context);

#endif /* APPS_MOTOR_PROC_H_ */
