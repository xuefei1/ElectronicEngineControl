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
 *
 */

#ifndef PROJ_CONFIG_H_
#define PROJ_CONFIG_H_
#include <stdlib.h>
#include <stdio.h>
#include "includes.h"
#include "system.h"
#include "altera_up_avalon_de0_nano_adc.h"

typedef INT8U BOOL;
#define TRUE 1
#define FALSE 0

/* Definition of Task Stacks */
#define TASK_STACKSIZE       2048

/* When APPS reading differ from last value by at least this much, we consider it as a new value */
#define APPS_VALUE_CHANGE_THRESHOLD			100

/* When two APPS reading differ by this much percent, we have a failure */
#define APPS_VALUE_DIFFERENCE_PERCENT		10

/* When two TPS reading differ by this much percent, we have a failure */
#define TPS_VALUE_DIFFERENCE_PERCENT		10

/* Actual TPS reading can be different from expect position by at most this much */
#define TPS_VALUE_TOLERANCE					50

/* 1000000 will given an accuracy of 6 significant figures when calculating percent diff of two integers */
#define PERCENT_DIFF_ACCURACY 				1000000

#define SEM_TIMEOUT_WAIT_FOREVER			0

#define Q_TIMEOUT_WAIT_FOREVER				0

/* Possible failure codes */
#define ERR_UNKNOWN							0
#define ERR_EXPECTED_THROTTLE_POS_MISMATCH	1
#define ERR_APPS_READING_MISMATCH			2
#define ERR_TPS_READING_MISMATCH			3

alt_up_de0_nano_adc_dev* get_adc();

void disp_err(INT8U err, char *msg);

/* Tasks use this Q to post failure codes to the failure handler */
OS_EVENT* get_failure_msg_q();

#endif /* PROJ_CONFIG_H_ */
