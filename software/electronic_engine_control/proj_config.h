/*
 * proj_config.h
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

#include "includes.h"
#include "system.h"

typedef INT8U BOOL;
#define TRUE 1
#define FALSE 0

/* Definition of Task Stacks */
#define   TASK_STACKSIZE       2048

/* When APPS reading differ from last value by at least this much, we consider it as a new value */
#define APPS_VALUE_CHANGE_THRESHOLD			100

/* When two APPS reading differ by this much percent, we have a failure */
#define APPS_VALUE_DIFFERENCE_PERCENT		10

/* 1000000 will given an accuracy of 6 significant figures when calculating percent diff of two integers */
#define PERCENT_DIFF_ACCURACY 				1000000

#endif /* PROJ_CONFIG_H_ */
