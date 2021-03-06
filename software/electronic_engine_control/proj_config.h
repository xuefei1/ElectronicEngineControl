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
#include <math.h>
#include "custom_types.h"
#include "system.h"
#include "altera_up_avalon_de0_nano_adc.h"
#include "sys/alt_timestamp.h"
#include "test_config.h"
#include "mem_manager.h"

/* Definition of Task Stacks */
#define TASK_STACKSIZE       					2048

/* 20kHz under a 50Mhz clock, yields 3125 clk ticks per period */
#define MOTOR_PWM_PERIOD_TICKS					2500
#define MOTOR_PWM_DUTY_CYCLE_FULLY_OPEN			300
#define MOTOR_PWM_DUTY_CYCLE_FULLY_CLOSE		580
#define MOTOR_PWM_DUTY_CYCLE_RESOLUTION			10
#define MOTOR_PWM_DUTY_CYCLE_RESOLUTION_HIGH	2

#define RPM_MATCHING_DELAY_COUNT				15

/* 16kHz under a 50Mhz clock, yields 3125 clk ticks per period */
#define TPS_OUT_PWM_PERIOD_TICKS				3125

/* APPS valid value range based on pedal travel */
#define APPS_VALID_VALUE_FULLY_RELEASED			660
#define APPS_VALID_VALUE_FULLY_PRESSED			3100

/* TPS valid value range based on throttle plate travel */
#define TPS_VALID_VALUE_FULLY_OPENED			3600
#define TPS_VALID_VALUE_FULLY_CLOSED			850

/* When APPS reading differ from last value by at least this much, we consider it as a new value */
#define APPS_VALUE_CHANGE_THRESHOLD				50

/* When two APPS reading differ by this much percent, we have a failure */
#define APPS_VALUE_DIFFERENCE_PERCENT			250

/* When two TPS reading differ by this much percent, we have a failure */
#define TPS_VALUE_DIFFERENCE_PERCENT			50

/* Actual TPS reading can be different from expected by at most this much */
#define TPS_VALUE_TOLERANCE						100

/* Two TPS readings can be different by at most this much */
#define TPS_VALUE_DIFFERENCE					5500

/* Actual RPM reading can be different from expected by at most this much */
#define RPM_VALUE_TOLERANCE						200

/* Detects slip if difference is beyond this percent */
#define WSS_VALUE_DIFFERENCE_PERCENT			50

/* In slip control, throttle will be set to this value */
#define SLIP_CONTROL_THROTTLE_POS				TPS_VALID_VALUE_FULLY_CLOSED

/* 1000000 will given an accuracy of 6 significant figures when calculating percent diff of two integers */
#define PERCENT_DIFF_ACCURACY 					1000000

/* Position sensor value comparison time : 100 ms */
#define SENSOR_VAL_COMP_DELAY_TICKS				100

/* Solenoids will open for 200 ms */
#define SOLENOID_OPEN_DURATION_TICKS			200

/* Time constraint for throttle plate to reach desired position */
#define MOTOR_DRIVE_DELAY_TICKS					2000

/* Time constraint for throttle plate to desired RPM */
#define MOTOR_RPM_DRIVE_DELAY_TICKS				5000

#define SEM_TIMEOUT_WAIT_FOREVER				0

#define Q_TIMEOUT_WAIT_FOREVER					0

#define MIN_THROTTLE_DEG						0

#define MAX_THROTTLE_DEG						90

/* Since we use semaphores as flags, a return value of 0 from OSSemAccpet() indicating no failure */
#define SEM_FLAG_NO_ERROR						0

/* Since we use semaphores as flags, a return value of 0 from OSSemAccpet() indicating current error is unresolved*/
#define SEM_FLAG_ERROR_UNRESOLVED				0

#define NO_ERROR_INDICATION_LED					0x0
#define MOTOR_FAILURE_INDICATION_LED			0x1
#define TPS_FAILURE_INDICATION_LED				0x2
#define APPS_FAILURE_INDICATION_LED				0x4
#define UNKNOWN_FAILURE_INDICATION_LED			0xF
#define WSS_ACTIVE_LED							0x80

/* Possible failure codes */
#define ERR_UNKNOWN								0
#define ERR_EXPECTED_THROTTLE_POS_MISMATCH		1
#define ERR_APPS_READING_MISMATCH				2
#define ERR_TPS_READING_MISMATCH				3

#define FAILURE_HANDLER_Q_SIZE_ELEMENTS			256

/* Time conversions */
#define TIME_1_MS_IN_US							1000

#define FLOAT_SCALE_FACTOR_10					10

#define FLOAT_SCALE_FACTOR_100					100

#define FLOAT_SCALE_FACTOR_1000					1000

#define SWITCH_ON								1

#define SWITCH_OFF								0

#define SWITCH_CLEAR_FAILURE_MASK				1

#define SWITCH_ENABLE_SHIFT_MATCHING_MASK		2

#define SWITCH_ENABLE_SLIP_CONTROL_MASK			4

#define SWITCH_ENABLE_SLIP_CONTROL_DEMO_MASK	8

#define SWITCH_ENABLE_SHIFT_MATCHING			2

#define SWITCH_ENABLE_SLIP_CONTROL				4

#define SWITCH_ENABLE_SLIP_CONTROL_DEMO			8

#define LED_FLASH_PERIOD_MS						500

#define PWM_490HZ_PERIOD_IN_20_NS_TICKS			102040

alt_up_de0_nano_adc_dev* get_adc();

/* Tasks use this Q to post failure codes to the failure handler */
OS_EVENT* get_failure_msg_q();

#endif /* PROJ_CONFIG_H_ */
