/*
 * solenoid_task.h
 *
 * Status: C
 *
 */

#ifndef SOLENOID_TASK_H_
#define SOLENOID_TASK_H_

#include "proj_config.h"
#include "apps_task.h"
#include "altera_avalon_pio_regs.h"
#include "eng_trans_data.h"
#include "pwm_gen.h"

#define SOLENOID_TASK_PRIORITY      		3

/* Task Delays*/
#define SOLENOID_TASK_DELAY_HOURS	  		0
#define SOLENOID_TASK_DELAY_MINUTES	  		0
#define SOLENOID_TASK_DELAY_SECONDS	  		0
#define SOLENOID_TASK_DELAY_MILLISEC	 	50

#define SOLENOID_Q_SIZE_ELEMENTS			256

#define DONE_SHIFTING						0
#define BUTTON_INPUT_SHIFT_UP				4
#define BUTTON_INPUT_SHIFT_DOWN				8
#define CLEAR_BUTTON_IRQ					128
#define CLEAR_BUTTON_EDGE_REG				0
#define CLEAR_BUTTON_DATA					0

#define OS_SEM_FLAG_SHIFTING				0
#define OS_SEM_FLAG_NOT_SHIFTING			1

void solenoid_task(void* pdata);

void signal_shift_start();

typedef struct{
	INT8U curr_gear;
	INT8U new_gear;
} shift_req;

OS_EVENT* get_solenoid_task_external_failure_flag();

OS_EVENT* get_solenoid_task_failure_resolved_flag();

#endif /* SOLENOID_TASK_H_ */
