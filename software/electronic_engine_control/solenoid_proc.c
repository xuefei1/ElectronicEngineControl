/*
 * solenoid_proc.c
 *
 * Status: C
 *
 *  Created on: Feb 16, 2017
 */

#include "solenoid_proc.h"

/* Used to store shift commands */
OS_EVENT 	*btn_input_q;

INT8U		solenoid_buf[SOLENOID_Q_SIZE_BYTE];

/* Used to fag alarm can be freed */
OS_EVENT 	*free_alarm_flag;

/* Flag indicating failures detected in other tasks */
OS_EVENT *external_failure_flag;

/* Flag indicating failure resolved */
OS_EVENT *failure_resolved_flag;

alt_alarm   *alarm;

static void isr_btn (void* context, alt_u32 id);

alt_u32 solenoid_callback(void* context);

void shift_up();

void shift_down();

/*  Task routine for solenoid */
void solenoid_task(void* pdata) {

	INT8U err;
	
	BOOL solenoid_open_timer_activated = FALSE;

	btn_input_q = OSQCreate((void*)solenoid_buf, SOLENOID_Q_SIZE_BYTE / sizeof(char));
	
	alt_irq_register(SOLENOID_CONTROLLER_0_AVALON_SLAVE_READ_IRQ,NULL,  &isr_btn);
	
	external_failure_flag = OSSemCreate(SEM_FLAG_NO_ERROR);

	failure_resolved_flag = OSSemCreate(SEM_FLAG_ERROR_UNRESOLVED);

#if defined(RUN_AVG_TASK_TIME_TEST)
	if(alt_timestamp_start()<0)
	{
		printf("No timestamp timer\n");
		return;
	}
	INT32U iteration_count = 0;
	INT32U end_tick = 0;
	INT32U start_tick = alt_timestamp();
#endif

	while(1){
#if defined(RUN_AVG_TASK_TIME_TEST)
		INT8U shift_command = BUTTON_INPUT_SHIFT_UP;
#else
		INT8U shift_command = *(INT8U *) OSQPend(btn_input_q, Q_TIMEOUT_WAIT_FOREVER, &err);
		if(OSSemAccept(external_failure_flag) != SEM_FLAG_NO_ERROR){
			printf("External failure, block apps_motor_task\n");
			OSSemPend(failure_resolved_flag, Q_TIMEOUT_WAIT_FOREVER, &err);
		}
#endif
		if (shift_command == BUTTON_INPUT_SHIFT_UP){
			if (solenoid_open_timer_activated == FALSE){
				solenoid_open_timer_activated = TRUE;
				shift_up();
				alarm = (alt_alarm*)malloc(sizeof(alt_alarm));
				alt_alarm_start(alarm, SOLENOID_OPEN_DURATION_TICKS, &solenoid_callback, NULL);
			}
		}
		else if (shift_command == BUTTON_INPUT_SHIFT_DOWN){
			if (solenoid_open_timer_activated == FALSE){
				solenoid_open_timer_activated = TRUE;
				shift_down();
				alarm = (alt_alarm*)malloc(sizeof(alt_alarm));
				alt_alarm_start(alarm, SOLENOID_OPEN_DURATION_TICKS, &solenoid_callback, NULL);
			}
		}else {
			printf("Unknown shift command received: %d\n", shift_command);
		}
#if defined(RUN_AVG_TASK_TIME_TEST)
		iteration_count++;
		if(iteration_count % AVG_ITERATION == 0){
			end_tick = alt_timestamp();
			printf("s%d:%d\n", iteration_count, (end_tick - start_tick)/iteration_count);
		}
#endif
		OSTimeDlyHMSM(SOLENOID_TASK_DELAY_HOURS,
				SOLENOID_TASK_DELAY_MINUTES,
				SOLENOID_TASK_DELAY_SECONDS,
				SOLENOID_TASK_DELAY_MILLISEC);
	}
}


void shift_up(){
	//write shift up command
	*(INT8U*) SOLENOID_CONTROLLER_0_AVALON_SLAVE_WRITE_BASE = BUTTON_INPUT_SHIFT_UP;
}

void shift_down(){
	//write shift down command
	*(INT8U*) SOLENOID_CONTROLLER_0_AVALON_SLAVE_WRITE_BASE = BUTTON_INPUT_SHIFT_DOWN;
}

/* Call back function after 200ms shift up or down */
alt_u32 solenoid_callback(void* context){
	//clear control
	if(alarm != NULL) free(alarm);
	return 0;
}

static void isr_btn (void* context, alt_u32 id)
{
	INT8U *shift = (INT8U*) SOLENOID_CONTROLLER_0_AVALON_SLAVE_READ_BASE;
	OSQPost(btn_input_q, (void*) shift);

}
