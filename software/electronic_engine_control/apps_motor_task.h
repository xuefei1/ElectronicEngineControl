/*
 * apps_motor_task.h
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

#ifndef APPS_MOTOR_TASK_H_
#define APPS_MOTOR_TASK_H_

#include "proj_config.h"
#include "solenoid_task.h"
#include "util.h"
#include "motor_control_task.h"
#include "throttle_data.h"

#define APPS_MOTOR_TASK_PRIORITY      		2

/* ADC channel assignments */
#define APPS_1_ADC_CHANNEL			  		0
#define APPS_2_ADC_CHANNEL			  		1

#define WSS_1_ADC_CHANNEL			  		4
#define WSS_2_ADC_CHANNEL			  		5

/* Task Delays*/
#define APPS_MOTOR_TASK_DELAY_HOURS	  		0
#define APPS_MOTOR_TASK_DELAY_MINUTES	  	0
#define APPS_MOTOR_TASK_DELAY_SECONDS	  	0
#define APPS_MOTOR_TASK_DELAY_MILLISEC	  	10

/* detect if there is a new APPS value*/
#define APPS_VALUE_CHANGED(input, last_value)	int16U_changed_by_threshold(input, last_value, APPS_VALUE_CHANGE_THRESHOLD)

/* detect if APPS reading differs by a percentage */
#define APPS_VALUE_MISMATCH(input1, input2)		int16U_differ_by_percent(input1, input2, APPS_VALUE_DIFFERENCE_PERCENT, PERCENT_DIFF_ACCURACY)

#define WSS_VALUE_MISMATCH(input1, input2)		int16U_differ_by_percent(input1, input2, WSS_VALUE_DIFFERENCE_PERCENT, PERCENT_DIFF_ACCURACY)

#define EXPECTED_TPS_READING_Q_SIZE_ELEMENTS		256

#define MOTOR_CMD_Q_SIZE_ELEMENTS					64

#define NEW_TPS_READING_SEM_COUNT					0

#define OS_SEM_RPM_NOT_REACHED						0

#define SHIFT_MATCHING_IN_PROGRESS					0

#define THROTTLE_POSITION_MAX						1000
#define THROTTLE_POSITION_MIN						0
#define ILLEGAL_THROTTLE_POSITION					0xFFFF

void apps_motor_task(void* pdata);

OS_EVENT* get_expected_motor_pos_q();

OS_EVENT* get_motor_cmd_q();

OS_EVENT* get_motor_failure_flag();

OS_EVENT* get_apps_motor_task_external_failure_flag();

OS_EVENT* get_apps_motor_task_failure_resolved_flag();

INT16U get_expected_tps_reading(INT16U apps_reading);

BOOL set_new_motor_position(INT16U *apps_reading);

BOOL set_new_motor_position_by_tps(INT16U *tps_reading);

INT32U hitec_servo_demo(INT16U commanded_position);

alt_u32 apps_value_comp_callback(void* context);

alt_u32 motor_pos_check_callback(void* context);

void signal_exit_shift_matching();

#endif /* APPS_MOTOR_TASK_H_ */
