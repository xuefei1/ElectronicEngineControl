/*
 * main.c
 *
 *  Created on: Feb 2, 2017
 *      Author: Fred
 *
 *      Entry point, responsible for creating all tasks
 *
 *      *See proj_config.h for acronym translations
 */

#include <stdio.h>
#include "proj_config.h"
#include "test_config.h"
#include "apps_motor_proc.h"

/* Definition of Task Stacks */
OS_STK apps_motor_task_stk[TASK_STACKSIZE];

/* The main function */
int main(void) {

#if defined(RUN_ALL_TESTS)
	run_all_tests();
#endif

	OSTaskCreateExt(apps_motor_task, NULL,
			(void *) &apps_motor_task_stk[TASK_STACKSIZE - 1],
			APPS_MOTOR_TASK_PRIORITY, APPS_MOTOR_TASK_PRIORITY,
			apps_motor_task_stk, TASK_STACKSIZE, NULL, 0);
	OSStart();

	return 0;
}
