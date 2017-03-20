/*
 * main.c
 *
 * Status: T
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
#include "apps_task.h"
#include "solenoid_task.h"
#include "failure_handler_task.h"
#include "motor_control_task.h"

/* Definition of Task Stacks */
OS_STK apps_task_stk[TASK_STACKSIZE];
OS_STK failure_handler_task_stk[TASK_STACKSIZE];
OS_STK solenoid_task_stk[TASK_STACKSIZE];
OS_STK motor_control_task_stk[TASK_STACKSIZE];

alt_up_de0_nano_adc_dev* adc;

/* Used to store failure code */
OS_EVENT *failure_code_q;

INT8U failure_code_buf[FAILURE_HANDLER_Q_SIZE_ELEMENTS];

/* The main function */
int main(void) {

	adc = alt_up_de0_nano_adc_open_dev(DE0_NANO_ADC_0_NAME);

	failure_code_q = OSQCreate((void*) failure_code_buf,
			FAILURE_HANDLER_Q_SIZE_ELEMENTS);
	printf("alive\n");

#if defined(RUN_UNIT_TESTS)
	printf("%d tests failed\n", run_all_unit_tests());
#endif

	throttle_data_init();

	TEST_PWM(3125, 56);

	OSTaskCreateExt(failure_handler_task, NULL,
			(void *) &failure_handler_task_stk[TASK_STACKSIZE - 1],
			FAILURE_HANDLER_TASK_PRIORITY, FAILURE_HANDLER_TASK_PRIORITY,
			failure_handler_task_stk, TASK_STACKSIZE, NULL, 0);

	OSTaskCreateExt(motor_control_task, NULL, (void *) &motor_control_task_stk[TASK_STACKSIZE - 1],
			MOTOR_CONTROL_TASK_PRIORITY, MOTOR_CONTROL_TASK_PRIORITY, motor_control_task_stk, TASK_STACKSIZE,
			NULL, 0);

	OSTaskCreateExt(apps_task, NULL,
			(void *) &apps_task_stk[TASK_STACKSIZE - 1],
			APPS_TASK_PRIORITY, APPS_TASK_PRIORITY,
			apps_task_stk, TASK_STACKSIZE, NULL, 0);

	OSTaskCreateExt(solenoid_task, NULL, (void *) &solenoid_task_stk[TASK_STACKSIZE - 1],
			SOLENOID_TASK_PRIORITY, SOLENOID_TASK_PRIORITY, solenoid_task_stk, TASK_STACKSIZE,
			NULL, 0);
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
