/*
 * main.c
 *
 * Status: C
 *
 *  Created on: Feb 2, 2017
 *      Author: Fred
 *
 *      Entry point, responsible for creating all tasks
 *
 *      *See proj_config.h for acronym translations
 */

#include "proj_config.h"
#include "test_config.h"
#include "apps_motor_proc.h"
#include "tps_proc.h"
#include "ui_proc.h"
#include "failure_handler_proc.h"

/* Definition of Task Stacks */
OS_STK apps_motor_task_stk[TASK_STACKSIZE];
OS_STK tps_task_stk[TASK_STACKSIZE];
OS_STK ui_task_stk[TASK_STACKSIZE];
OS_STK failure_handler_task_stk[TASK_STACKSIZE];

alt_up_de0_nano_adc_dev* adc;

/* Used to store failure code */
OS_EVENT *failure_code_q;

INT8U failure_code_buf[FAILURE_HANDLER_Q_SIZE_BYTE];

/* The main function */
int main(void) {

	adc = alt_up_de0_nano_adc_open_dev(DE0_NANO_ADC_0_NAME);

	failure_code_q = OSQCreate((void*) failure_code_buf,
			FAILURE_HANDLER_Q_SIZE_BYTE / sizeof(INT16U));

#if defined(RUN_ALL_TESTS)
	printf("%d tests failed\n", run_all_tests());
#endif
	printf("alive\n");
	OSTaskCreateExt(failure_handler_task, NULL,
			(void *) &failure_handler_task_stk[TASK_STACKSIZE - 1],
			FAILURE_HANDLER_TASK_PRIORITY, FAILURE_HANDLER_TASK_PRIORITY,
			failure_handler_task_stk, TASK_STACKSIZE, NULL, 0);

	OSTaskCreateExt(apps_motor_task, NULL,
			(void *) &apps_motor_task_stk[TASK_STACKSIZE - 1],
			APPS_MOTOR_TASK_PRIORITY, APPS_MOTOR_TASK_PRIORITY,
			apps_motor_task_stk, TASK_STACKSIZE, NULL, 0);

	OSTaskCreateExt(tps_task, NULL, (void *) &tps_task_stk[TASK_STACKSIZE - 1],
			TPS_TASK_PRIORITY, TPS_TASK_PRIORITY, tps_task_stk, TASK_STACKSIZE,
			NULL, 0);
//
//	OSTaskCreateExt(ui_task, NULL, (void *) &ui_task_stk[TASK_STACKSIZE - 1],
//			UI_TASK_PRIORITY, UI_TASK_PRIORITY, ui_task_stk, TASK_STACKSIZE,
//			NULL, 0);

	OSStart();

	return 0;
}

/* Get ADC controller instance */
alt_up_de0_nano_adc_dev* get_adc() {
	return adc;
}

/* Display error message */
void disp_err(INT8U err, char *msg) {
	printf("%d:%s\n", err, msg);
}

/* Getter for failure_message Q for every other task to use */
OS_EVENT* get_failure_msg_q() {
	return failure_code_q;
}
