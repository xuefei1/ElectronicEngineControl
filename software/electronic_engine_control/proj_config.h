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

/* 16kHz under a 50Mhz clock, yields 3125 clk ticks per period */
#define MOTOR_PWM_PERIOD_TICKS				3125
#define MOTOR_PWM_DUTY_CYCLE_IDLE			50
#define MOTOR_PWM_DUTY_CYCLE_CW				100
#define MOTOR_PWM_DUTY_CYCLE_CCW			0

/* 16kHz under a 50Mhz clock, yields 3125 clk ticks per period */
#define TPS_OUT_PWM_PERIOD_TICKS			3125

/* APPS valid value range based on pedal travel, note this is the average of two sensors */
#define APPS_VALID_VALUE_MIN				600
#define APPS_VALID_VALUE_MAX				1750

/* TPS valid value range based on throttle plate travel, note this is the average of two sensors */
#define TPS_VALID_VALUE_MIN					450
#define TPS_VALID_VALUE_MAX					2400

/* When APPS reading differ from last value by at least this much, we consider it as a new value */
#define APPS_VALUE_CHANGE_THRESHOLD			30

/* When two APPS reading differ by this much percent, we have a failure */
#define APPS_VALUE_DIFFERENCE_PERCENT		180

/* When two TPS reading differ by this much percent, we have a failure */
#define TPS_VALUE_DIFFERENCE_PERCENT		10

/* Actual TPS reading can be different from expected by at most this much */
#define TPS_VALUE_TOLERANCE					80

/* Two TPS readings can be different by at most this much */
#define TPS_VALUE_DIFFERENCE				2000

/* Actual RPM reading can be different from expected by at most this much */
#define RPM_VALUE_TOLERANCE					150

/* Detects slip if difference is beyond this percent */
#define WSS_VALUE_DIFFERENCE_PERCENT		30

/* In slip control, throttle will be set to this value */
#define SLIP_CONTROL_THROTTLE_POS			20

/* 1000000 will given an accuracy of 6 significant figures when calculating percent diff of two integers */
#define PERCENT_DIFF_ACCURACY 				1000000

/* Position sensor value comparison time : 100 ms */
#define SENSOR_VAL_COMP_DELAY_TICKS			100

/* Solenoids will open for 200 ms */
#define SOLENOID_OPEN_DURATION_TICKS		200

/* Time constraint for throttle plate to reach desired position: 1 second */
#define MOTOR_DRIVE_DELAY_TICKS				1000

/* Time constraint for throttle plate to desired RPM: 2 second */
#define MOTOR_RPM_DRIVE_DELAY_TICKS			2000

#define SEM_TIMEOUT_WAIT_FOREVER			0

#define Q_TIMEOUT_WAIT_FOREVER				0

#define MAX_THROTTLE_DEG					90

#define MAX_SCALED_RPM						60

#define RPM_SCALE_FACTOR					100

#define NUM_GEARS							4

#define GEAR_RATIO_1ST						18

#define GEAR_RATIO_2ND						12

#define GEAR_RATIO_3RD						10

#define GEAR_RATIO_4TH						8

/* Since we use semaphores as flags, a return value of 0 from OSSemAccpet() indicating no failure */
#define SEM_FLAG_NO_ERROR					0

/* Since we use semaphores as flags, a return value of 0 from OSSemAccpet() indicating current error is unresolved*/
#define SEM_FLAG_ERROR_UNRESOLVED			0

#define NO_ERROR_INDICATION_LED				0x0
#define MOTOR_FAILURE_INDICATION_LED		0x1
#define TPS_FAILURE_INDICATION_LED			0x2
#define APPS_FAILURE_INDICATION_LED			0x4
#define UNKNOWN_FAILURE_INDICATION_LED		0xF
#define WSS_ACTIVE_LED						0x80

/* Possible failure codes */
#define ERR_UNKNOWN							0
#define ERR_EXPECTED_THROTTLE_POS_MISMATCH	1
#define ERR_APPS_READING_MISMATCH			2
#define ERR_TPS_READING_MISMATCH			3

#define FAILURE_HANDLER_Q_SIZE_ELEMENTS		256

/* Time conversions */
#define TIME_1_MS_IN_US						1000

#define FLOAT_SCALE_FACTOR_10				10

#define FLOAT_SCALE_FACTOR_100				100

#define SWITCH_ON							1

#define SWITCH_OFF							0

#define LED_FLASH_PERIOD_MS					500

alt_up_de0_nano_adc_dev* get_adc();

void disp_err(INT8U err, char *msg);

/* Tasks use this Q to post failure codes to the failure handler */
OS_EVENT* get_failure_msg_q();

INT8U get_gear_ratio(INT8U gear);

#endif /* PROJ_CONFIG_H_ */
