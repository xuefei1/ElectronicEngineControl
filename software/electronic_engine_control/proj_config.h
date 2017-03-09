/*
 * proj_config.h
 *
 * Status: C
 *
 * 	Created on: Feb 2, 2017
 *      Author: Fred
 *
 *  Header containing global project config
 *
 *  *Acronym translations:
 *  APPS - Accelerator Pedal Position Sensor
 *  TPS - Throttle Position Sensor
 *  WSS - Wheel Speed Sensor
 *
 */

#ifndef PROJ_CONFIG_H_
#define PROJ_CONFIG_H_
#include <stdlib.h>
#include <stdio.h>
#include "custom_types.h"
#include "system.h"
#include "altera_up_avalon_de0_nano_adc.h"
#include "sys/alt_timestamp.h"
#include "test_config.h"
#include "throttle_data.h"

/* Definition of Task Stacks */
#define TASK_STACKSIZE       				2048

/* When APPS reading differ from last value by at least this much, we consider it as a new value */
#define APPS_VALUE_CHANGE_THRESHOLD			100

/* Natural difference in two APPS sensors */
#define APPS_SENSOR_OFFSET					900

/* When two APPS reading differ by this much percent, we have a failure */
#define APPS_VALUE_DIFFERENCE_PERCENT		10

/* When two TPS reading differ by this much percent, we have a failure */
#define TPS_VALUE_DIFFERENCE_PERCENT		10

/* Actual TPS reading can be different from expect position by at most this much */
#define TPS_VALUE_TOLERANCE					50

/* 1000000 will given an accuracy of 6 significant figures when calculating percent diff of two integers */
#define PERCENT_DIFF_ACCURACY 				1000000

/* Position sensor value comparison time : 100 ms */
#define SENSOR_VAL_COMP_DELAY_TICKS			100

/* Solenoids will open for 200 ms */
#define SOLENOID_OPEN_DURATION_TICKS		200

/* Time constraint for throttle plate to reach desired position: 1 second */
#define MOTOR_DRIVE_DELAY_TICKS				1000

#define SEM_TIMEOUT_WAIT_FOREVER			0

#define Q_TIMEOUT_WAIT_FOREVER				0

#define MAX_THROTTLE_DEG					90

#define MAX_SCALED_RPM						60

#define NUM_GEARS							4

#define GEAR_RATIO_1ST						18

#define GEAR_RATIO_2ND						12

#define GEAR_RATIO_3RD						10

#define GEAR_RATIO_4TH						8

/* Since we use semaphores as flags, a return value of 0 from OSSemAccpet() indicating no failure */
#define SEM_FLAG_NO_ERROR					0

/* Since we use semaphores as flags, a return value of 0 from OSSemAccpet() indicating current error is unresolved*/
#define SEM_FLAG_ERROR_UNRESOLVED			0

/* Possible failure codes */
#define ERR_UNKNOWN							0
#define ERR_EXPECTED_THROTTLE_POS_MISMATCH	1
#define ERR_APPS_READING_MISMATCH			2
#define ERR_TPS_READING_MISMATCH			3

#define FAILURE_HANDLER_Q_SIZE_ELEMENTS		256

/* Time conversions */
#define TIME_1_MS_IN_US						1000

alt_up_de0_nano_adc_dev* get_adc();

void disp_err(INT8U err, char *msg);

/* Tasks use this Q to post failure codes to the failure handler */
OS_EVENT* get_failure_msg_q();

INT8U get_gear_ratio(INT8U gear);

#endif /* PROJ_CONFIG_H_ */
