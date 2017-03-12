/*
 * solenoid_proc.h
 *
 * Status: C
 *
 */

#ifndef SOLENOID_PROC_H_
#define SOLENOID_PROC_H_

#include "proj_config.h"
#include "apps_motor_proc.h"
#include "altera_avalon_pio_regs.h"

#define SOLENOID_TASK_PRIORITY      		4

/* Task Delays*/
#define SOLENOID_TASK_DELAY_HOURS	  		0
#define SOLENOID_TASK_DELAY_MINUTES	  		0
#define SOLENOID_TASK_DELAY_SECONDS	  		0
#define SOLENOID_TASK_DELAY_MILLISEC	 	10

#define SOLENOID_Q_SIZE_ELEMENTS			256

#define DONE_SHIFTING						0
#define BUTTON_INPUT_SHIFT_UP				4
#define BUTTON_INPUT_SHIFT_DOWN				8
#define CLEAR_BUTTON_IRQ					128

#define OS_SEM_FLAG_SHIFTING				0
#define OS_SEM_FLAG_NOT_SHIFTING			1

void solenoid_task(void* pdata);

void signal_shift_start();

OS_EVENT* get_solenoid_task_external_failure_flag();

OS_EVENT* get_solenoid_task_failure_resolved_flag();

#endif /* SOLENOID_PROC_H_ */
