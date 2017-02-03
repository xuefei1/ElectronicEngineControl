/*
 * apps_motor_proc.h
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

#define APPS_MOTOR_TASK_PRIORITY      		1

/* ADC channel assignments */
#define APPS_1_ADC_CHANNEL			  		0
#define APPS_2_ADC_CHANNEL			  		1

/* Task Delays*/
#define APPS_MOTOR_TASK_DELAY_HOURS	  		0
#define APPS_MOTOR_TASK_DELAY_MINUTES	  	0
#define APPS_MOTOR_TASK_DELAY_SECONDS	  	0
#define APPS_MOTOR_TASK_DELAY_MILLISEC	  	100
void apps_motor_task(void* pdata);

#endif /* APPS_MOTOR_PROC_H_ */
