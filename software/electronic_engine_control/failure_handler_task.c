/*
 * failure_handler_task.c
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

#include "failure_handler_task.h"

/*  Task routine for handling any failures motors/sensors */
void failure_handler_task(void* pdata) {

	INT8U err;
	OS_EVENT *failure_code_q = get_failure_msg_q();
	*(INT8U*)GREEN_LEDS_BASE = NO_ERROR_INDICATION_LED;
	INT8U error_led = NO_ERROR_INDICATION_LED;
	INT8U next_clear_switch_val = *(INT8U*)SWITCH_BASE == SWITCH_OFF ? SWITCH_ON : SWITCH_OFF;
	while (1) {
		INT8U msg = (INT8U) OSQPend(failure_code_q, Q_TIMEOUT_WAIT_FOREVER, &err);
		if (err) printf("error %d pending on failure q\n", err);

		switch (msg) {

		case ERR_EXPECTED_THROTTLE_POS_MISMATCH:
			printf("Possible motor failure detected!\n");
			if(OSSemAccept(get_solenoid_task_external_failure_flag()) == SEM_FLAG_NO_ERROR)
				OSSemPost(get_solenoid_task_external_failure_flag());
			error_led = MOTOR_FAILURE_INDICATION_LED;
			break;

		case ERR_APPS_READING_MISMATCH:
			printf("Possible APPS failure detected!\n");
			if(OSSemAccept(get_solenoid_task_external_failure_flag()) == SEM_FLAG_NO_ERROR)
				OSSemPost(get_solenoid_task_external_failure_flag());
			error_led = APPS_FAILURE_INDICATION_LED;
			break;

		case ERR_TPS_READING_MISMATCH:
			printf("Possible TPS failure detected!\n");
			if(OSSemAccept(get_solenoid_task_external_failure_flag()) == SEM_FLAG_NO_ERROR)
				OSSemPost(get_solenoid_task_external_failure_flag());
			error_led = TPS_FAILURE_INDICATION_LED;
			break;

		case ERR_UNKNOWN:
			printf("Unknown failure detected!\n");
			if(OSSemAccept(get_solenoid_task_external_failure_flag()) == SEM_FLAG_NO_ERROR)
				OSSemPost(get_solenoid_task_external_failure_flag());
			error_led = UNKNOWN_FAILURE_INDICATION_LED;
			break;

		default:
			printf("Unknown failure code\n");
			if(OSSemAccept(get_solenoid_task_external_failure_flag()) == SEM_FLAG_NO_ERROR)
				OSSemPost(get_solenoid_task_external_failure_flag());
			error_led = UNKNOWN_FAILURE_INDICATION_LED;
			break;
		}
		//wait for outside command
		while(*(INT8U*)SWITCH_BASE != next_clear_switch_val){
			*(INT8U*)GREEN_LEDS_BASE = *(INT8U*)GREEN_LEDS_BASE == NO_ERROR_INDICATION_LED ? error_led : NO_ERROR_INDICATION_LED;
			OSTimeDlyHMSM(FAILURE_HANDLER_TASK_DELAY_HOURS,
					FAILURE_HANDLER_TASK_DELAY_MINUTES,
					FAILURE_HANDLER_TASK_DELAY_SECONDS,
					LED_FLASH_PERIOD_MS);
		}
		next_clear_switch_val = *(INT8U*)SWITCH_BASE == SWITCH_OFF ? SWITCH_ON : SWITCH_OFF;
		*(INT8U*)GREEN_LEDS_BASE = NO_ERROR_INDICATION_LED;
		if(OSSemAccept(get_throttle_control_task_failure_resolved_flag()) == SEM_FLAG_ERROR_UNRESOLVED)
			OSSemPost(get_throttle_control_task_failure_resolved_flag());
		if(OSSemAccept(get_solenoid_task_failure_resolved_flag()) == SEM_FLAG_ERROR_UNRESOLVED)
			OSSemPost(get_solenoid_task_failure_resolved_flag());
		OSTimeDlyHMSM(FAILURE_HANDLER_TASK_DELAY_HOURS,
				FAILURE_HANDLER_TASK_DELAY_MINUTES,
				FAILURE_HANDLER_TASK_DELAY_SECONDS,
				FAILURE_HANDLER_TASK_DELAY_MILLISEC);
	}
}

