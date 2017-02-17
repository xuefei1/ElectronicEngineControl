/*
 * solenoid.c
 *
 * Status: C
 *
 *  Created on: Feb 16, 2017
 */

#include "solenoid.h"

/* Used to store solenoid code */
OS_EVENT *solenoid_q;

char		solenoid_buf[Q_SIZE_BYTES];

/*  Task routine for solenoid */
void solenoid_task(void* pdata) {

	INT8U err;
	
	BOOL solenoid_check_timer_activated = FALSE;
	
	solenoid_q = OSQCreate((void*)solenoid_buf, SOLENOID_Q_SIZE_BYTE / sizeof(char));
	INT16U shift_state = *(INT16U *) OSQPend(solenoid_q, Q_TIMEOUT_WAIT_FOREVER, &err);
	
	//have question about GATE_TIMER_IRQ_INTERRUPT_CONTROLLER_ID and GATE_TIMER_IRQ
	alt_ic_isr_register(GATE_TIMER_IRQ_INTERRUPT_CONTROLLER_ID, GATE_TIMER_IRQ, &shift_up, NULL, NULL);
	alt_ic_isr_register(GATE_TIMER_IRQ_INTERRUPT_CONTROLLER_ID, GATE_TIMER_IRQ, &shift_down, NULL, NULL);
	
	if (shift_state == shift_up){
		if (solenoid_check_timer_activated = FALSE){
			solenoid_check_timer_activated = TRUE;
			printf("set alarm\n");
			alarm = (alt_alarm*)malloc(sizeof(alt_alarm));
			alt_alarm_start(alarm, SOLENOID_FIRE_DELAY, &solenoid_callback, NULL);
		}
	}
	else if (shift_state == shift_down){
		if (solenoid_check_timer_activated = FALSE){
			solenoid_check_timer_activated = TRUE;
			printf("set alarm\n");
			alarm = (alt_alarm*)malloc(sizeof(alt_alarm));
			alt_alarm_start(alarm, SOLENOID_FIRE_DELAY, &solenoid_callback, NULL);
		}
	}else {
		if (solenoid_check_timer_activated = TRUE){
			printf("clear alarm\n");
			alt_alarm_stop (alarm);
			free(alarm);
			solenoid_check_timer_activated = FALSE;
		}
	}
	OSTimeDlyHMSM(SOLENOID_TASK_DELAY_HOURS,
			SOLENOID_TASK_DELAY_MINUTES,
			SOLENOID_TASK_DELAY_SECONDS,
			SOLENOID_TASK_DELAY_MILLISEC);
}


static void shift_up(void *context){
	OSQPost(solenoid_q, (void*) shift_up);
}

static void shift_down(void *context){
	OSQPost(solenoid_q, (void*) shift_down);
}

/* Call back function after 200ms shift up or down timeout */
alt_u32 solenoid_callback(void* context){
	
	return 0;
}