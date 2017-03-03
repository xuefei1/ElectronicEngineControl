/*
 * solenoid_proc.h
 *
 * Status: C
 *
 */

#ifndef SOLENOID_PROC_H_
#define SOLENOID_PROC_H_

#include "proj_config.h"

#define SOLENOID_TASK_PRIORITY      		4

/* Task Delays*/
#define SOLENOID_TASK_DELAY_HOURS	  		0
#define SOLENOID_TASK_DELAY_MINUTES	  		0
#define SOLENOID_TASK_DELAY_SECONDS	  		0
#define SOLENOID_TASK_DELAY_MILLISEC	 	100

#define SOLENOID_Q_SIZE_BYTE				256

#define BUTTON_INPUT_SHIFT_UP				1
#define BUTTON_INPUT_SHIFT_DOWN				2

void solenoid_task(void* pdata);

#endif /* SOLENOID_PROC_H_ */
