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

OS_EVENT *failure_resolved_flag;

/*  Task routine for handling any failures motors/sensors */
void failure_handler_task(void* pdata) {

	INT8U err;
	OS_EVENT *failure_code_q = get_failure_msg_q();
	failure_resolved_flag = OSSemCreate(SEM_FLAG_ERROR_UNRESOLVED);

	while (1) {
		INT8U msg = (INT8U) OSQPend(failure_code_q, Q_TIMEOUT_WAIT_FOREVER,
				&err);
		if (err)
			disp_err(err, "Error pending on q");

		//Routing table for all possible errors
		switch (msg) {

		case ERR_EXPECTED_THROTTLE_POS_MISMATCH:
			printf("Possible motor failure detected!\n");
			OSSemPost(get_tps_task_external_failure_flag());
			OSSemPost(get_solenoid_task_external_failure_flag());
			break;

		case ERR_APPS_READING_MISMATCH:
			printf("Possible APPS failure detected!\n");
			OSSemPost(get_tps_task_external_failure_flag());
			OSSemPost(get_solenoid_task_external_failure_flag());
			break;

		case ERR_TPS_READING_MISMATCH:
			printf("Possible TPS failure detected!\n");
			OSSemPost(get_apps_motor_task_external_failure_flag());
			OSSemPost(get_solenoid_task_external_failure_flag());
			break;

		case ERR_UNKNOWN:
			printf("Unknown failure detected!\n");
			OSSemPost(get_apps_motor_task_external_failure_flag());
			OSSemPost(get_solenoid_task_external_failure_flag());
			OSSemPost(get_tps_task_external_failure_flag());
			break;

		default:
			printf("Unknown failure code\n");
			OSSemPost(get_apps_motor_task_external_failure_flag());
			OSSemPost(get_solenoid_task_external_failure_flag());
			OSSemPost(get_tps_task_external_failure_flag());
			break;
		}
		//wait for outside command
		*(INT8U*)GREEN_LEDS_BASE = FAILURE_INDICATION_LED;
		OSSemPend(failure_resolved_flag, SEM_TIMEOUT_WAIT_FOREVER, &err);
		*(INT8U*)GREEN_LEDS_BASE = 0;
		OSSemPost(get_apps_motor_task_failure_resolved_flag());
		OSSemPost(get_solenoid_task_failure_resolved_flag());
		OSSemPost(get_tps_task_failure_resolved_flag());

		OSTimeDlyHMSM(FAILURE_HANDLER_TASK_DELAY_HOURS,
				FAILURE_HANDLER_TASK_DELAY_MINUTES,
				FAILURE_HANDLER_TASK_DELAY_SECONDS,
				FAILURE_HANDLER_TASK_DELAY_MILLISEC);
	}
}

