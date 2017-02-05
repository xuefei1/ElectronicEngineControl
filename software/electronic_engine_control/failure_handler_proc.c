/*
 * failure_handler_proc.c
 *
 * Status: C
 *
 *  Created on: Feb 4, 2017
 *      Author: xuefei1
 *
 *      Handling any failures of motors/sensors
 *
 *      *See proj_config.h for acronym translations
 */

#include "failure_handler_proc.h"

/* Used to store failure code */
OS_EVENT *failure_code_q;

char failure_code_buf[FAILURE_HANDLER_Q_SIZE_BYTE];

/*  Task routine for handling any failures motors/sensors */
void fault_handler_task(void* pdata) {

	INT8U err;
	failure_code_q = OSQCreate((void*) failure_code_buf,
			FAILURE_HANDLER_Q_SIZE_BYTE / sizeof(INT16U));
	while (1) {
		INT16U msg = *(INT16U *) OSQPend(failure_code_q, Q_TIMEOUT_WAIT_FOREVER,
				&err);
		if (err)
			disp_err(err, "Error pending on q");

		//Routing table for all possible errors
		switch (msg) {

		case ERR_EXPECTED_THROTTLE_POS_MISMATCH:

			break;

		case ERR_APPS_READING_MISMATCH:

			break;

		case ERR_TPS_READING_MISMATCH:

			break;

		case ERR_UNKNOWN:

			break;

		default:

			break;

		}
		OSTimeDlyHMSM(FAILURE_HANDLER_TASK_DELAY_HOURS,
				FAILURE_HANDLER_TASK_DELAY_MINUTES,
				FAILURE_HANDLER_TASK_DELAY_SECONDS,
				FAILURE_HANDLER_TASK_DELAY_MILLISEC);
	}
}

/* Getter for failure_message Q for every other task to use */
OS_EVENT* get_failure_msg_q() {
	return failure_code_q;
}
