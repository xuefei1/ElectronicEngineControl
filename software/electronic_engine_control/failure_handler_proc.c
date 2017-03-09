/*
 * failure_handler_proc.c
 *
 * Status: T
 *
 *  Created on: Feb 4, 2017
 *      Author: xuefei1
 *
 *      Handling any failures of motors/sensors
 *
 *      *See proj_config.h for acronym translations
 */

#include "failure_handler_proc.h"

/*  Task routine for handling any failures motors/sensors */
void failure_handler_task(void* pdata) {

	INT8U err;
	OS_EVENT *failure_code_q = get_failure_msg_q();
	while (1) {
		INT8U msg = (INT8U) OSQPend(failure_code_q, Q_TIMEOUT_WAIT_FOREVER,
				&err);
		if (err)
			disp_err(err, "Error pending on q");

		//Routing table for all possible errors
		switch (msg) {

		case ERR_EXPECTED_THROTTLE_POS_MISMATCH:
			printf("Possible motor failure detected!\n");
			break;

		case ERR_APPS_READING_MISMATCH:
			printf("Possible APPS failure detected!\n");
			break;

		case ERR_TPS_READING_MISMATCH:
			printf("Possible TPS failure detected!\n");
			break;

		case ERR_UNKNOWN:
			printf("Unknown failure detected!\n");
			break;

		default:
			printf("Unknow failure code\n");
			break;

		}
		OSTimeDlyHMSM(FAILURE_HANDLER_TASK_DELAY_HOURS,
				FAILURE_HANDLER_TASK_DELAY_MINUTES,
				FAILURE_HANDLER_TASK_DELAY_SECONDS,
				FAILURE_HANDLER_TASK_DELAY_MILLISEC);
	}
}

