/*
 * throttle_control_task.h
 *
 * Status: C
 *
 *  Created on: Feb 2, 2017
 *      Author: Fred
 *
 *
 *      *See proj_config.h for acronym translations
 */

#ifndef THROTTLE_CONTROL_TASK_H_
#define THROTTLE_CONTROL_TASK_H_

#include "proj_config.h"
#include "solenoid_task.h"
#include "util.h"
#include "throttle_data.h"
#include "altera_avalon_timer.h"
#include "eng_trans_data.h"

#define THROTTLE_CONTROL_TASK_PRIORITY      			3

/* ADC channel assignments */
#define APPS_1_ADC_CHANNEL			  					0
#define APPS_2_ADC_CHANNEL			  					1
#define TPS_1_ADC_CHANNEL			  					2
#define TPS_2_ADC_CHANNEL			  					3
#define WS_1_ADC_CHANNEL			  					4
#define WS_2_ADC_CHANNEL			  					5

/* Task Delays*/
#define THROTTLE_CONTROL_TASK_DELAY_HOURS	  			0
#define THROTTLE_CONTROL_TASK_DELAY_MINUTES	  			0
#define THROTTLE_CONTROL_TASK_DELAY_SECONDS	  			0
#define THROTTLE_CONTROL_TASK_DELAY_MILLISEC	  		5

#define MOTOR_CMD_Q_SIZE_ELEMENTS						64

#define OS_SEM_RPM_NOT_REACHED							0

#define SHIFT_MATCHING_IN_PROGRESS						0
#define SHIFT_MATCHING_RESULT_OK						1
#define SHIFT_MATCHING_RESULT_FAIL						2

/* detect if there is a new APPS value*/
#define APPS_VALUE_CHANGED(input, last_value)	int32U_changed_by_threshold(input, last_value, APPS_VALUE_CHANGE_THRESHOLD)

/* detect if APPS reading differs by a percentage */
#define APPS_VALUE_MISMATCH(input1, input2)		int32U_differ_by_percent(input1, input2, APPS_VALUE_DIFFERENCE_PERCENT, PERCENT_DIFF_ACCURACY)

#define WS_VALUE_MISMATCH(input1, input2)		int32U_differ_by_percent(input1, input2, WSS_VALUE_DIFFERENCE_PERCENT, PERCENT_DIFF_ACCURACY)

/* detect if TPS expected and actual value differ */
#define TPS_VALUE_DIFFER_FROM_EXPECTED(input, exp)	int32U_changed_by_threshold(input, exp, TPS_VALUE_TOLERANCE)
/* detect if RPM expected and actual value differ */
#define RPM_VALUE_DIFFER_FROM_EXPECTED(input, exp)	int32U_changed_by_threshold(input, exp, RPM_VALUE_TOLERANCE)
/* detect if TPS readings differ by a percentage */
#define TPS_VALUE_MISMATCH(input1, input2)			int32U_changed_by_threshold(input1, input2, TPS_VALUE_DIFFERENCE)

void throttle_control_task(void* pdata);

OS_EVENT* get_shift_matching_q();

OS_EVENT* get_throttle_control_task_external_failure_flag();

OS_EVENT* get_throttle_control_task_failure_resolved_flag();

void signal_exit_shift_matching();

#endif /* THROTTLE_CONTROL_TASK_H_ */
