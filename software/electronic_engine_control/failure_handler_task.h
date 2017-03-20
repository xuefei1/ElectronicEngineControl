/*
 * failure_handler_task.h
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

#ifndef FAILURE_HANDLER_TASK_H_
#define FAILURE_HANDLER_TASK_H_

#include "proj_config.h"
#include "apps_task.h"
#include "solenoid_task.h"

#define FAILURE_HANDLER_TASK_PRIORITY      				1

/* Task Delays*/
#define FAILURE_HANDLER_TASK_DELAY_HOURS	  			0
#define FAILURE_HANDLER_TASK_DELAY_MINUTES	  			0
#define FAILURE_HANDLER_TASK_DELAY_SECONDS	  			0
#define FAILURE_HANDLER_TASK_DELAY_MILLISEC	  			100

#define FAILURE_CLEAR_SWITCH_IRQ_MASK					1
#define FAILURE_CLEAR_SWITCH_EDGE_CAP_CLEAR				0

void failure_handler_task(void* pdata);

#endif /* FAILURE_HANDLER_TASK_H_ */
