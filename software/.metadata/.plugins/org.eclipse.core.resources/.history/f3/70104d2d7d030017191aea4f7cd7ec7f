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

alt_alarm   *alarm;

static void isr_btn (void* context);

alt_u32 solenoid_callback(void* context);

void shift_up();

void shift_down();

/*  Task routine for solenoid */
void solenoid_task(void* pdata) {

	INT8U err;
	
	BOOL solenoid_open_timer_activated = FALSE;

	btn_input_q = OSQCreate((void*)solenoid_buf, SOLENOID_Q_SIZE_BYTE / sizeof(char));
	
	alt_ic_isr_register(SOLENOID_CONTROLLER_0_AVALON_SLAVE_READ_IRQ, SOLENOID_CONTROLLER_0_AVALON_SLAVE_READ_IRQ_INTERRUPT_CONTROLLER_ID, &isr_btn, NULL, NULL);
	
	while(1){

		INT8U shift_command = *(INT8U *) OSQPend(btn_input_q, Q_TIMEOUT_WAIT_FOREVER, &err);
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

static void isr_btn (void* context)
{
	INT8U *shift = (INT8U*) SOLENOID_CONTROLLER_0_AVALON_SLAVE_READ_BASE;
	OSQPost(btn_input_q, (void*) shift);

}
