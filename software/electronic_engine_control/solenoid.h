/*
 * failure_handler_proc.h
 *
 * Status: C
 *
 *  Created on: Feb 4, 2017
 *      Author: xuefei1
 *
 *      Interface for fault_handler_proc
 *
 *      *See proj_config.h for acronym translations
 */

#ifndef SOLENOID_H_
#define SOLENOID_H_

#include "proj_config.h"

#define SOLENOID_TASK_PRIORITY      		4

/* Task Delays*/
#define SOLENOID_TASK_DELAY_HOURS	  		0
#define SOLENOID_TASK_DELAY_MINUTES	  	0
#define SOLENOID_TASK_DELAY_SECONDS	  	0
#define SOLENOID_TASK_DELAY_MILLISEC	 	100

#define SOLENOID_Q_SIZE_BYTE						256

#define shift_up												1
#define shift_down											0

void solenoid_task(void* pdata);

#endif /* SOLENOID_H_ */
