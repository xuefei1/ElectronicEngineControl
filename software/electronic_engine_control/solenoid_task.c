/*
 * solenoid_task.c
 *
 * Status: C
 *
 *  Created on: Feb 16, 2017
 */

#include "solenoid_task.h"

/* Used to store shift commands */
OS_EVENT 	*btn_input_q;

INT8U		solenoid_btn_buf[SOLENOID_Q_SIZE_ELEMENTS];
INT8U		shift_matching_result_buf[SOLENOID_Q_SIZE_ELEMENTS];

/* Used to fag alarm can be freed */
OS_EVENT 	*free_alarm_flag;

/* Flag indicating failures detected in other tasks */
OS_EVENT 	*solenoid_external_failure_flag;

/* Flag indicating failure resolved */
OS_EVENT 	*solenoid_failure_resolved_flag;

/* Flag indicating target RPM reached */
OS_EVENT 	*rpm_matching_result_q;

static void isr_btn (void* context, alt_u32 id)
{
	static INT32U data = 0;
	data = IORD_ALTERA_AVALON_PIO_EDGE_CAP(BUTTONS_BASE);
	INT32U *ptr = &data;
	if(data == BUTTON_INPUT_SHIFT_UP){
		OSQPost(btn_input_q, (void*) ptr);
	}else if(data == BUTTON_INPUT_SHIFT_DOWN){
		OSQPost(btn_input_q, (void*) ptr);
	}
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(BUTTONS_BASE, CLEAR_BUTTON_EDGE_REG);
}

void shift_up(){
	//write shift up command
	IOWR_ALTERA_AVALON_PIO_DATA(SOLENOID_OUT_BASE, BUTTON_INPUT_SHIFT_UP);
}

void shift_down(){
	//write shift down command
	IOWR_ALTERA_AVALON_PIO_DATA(SOLENOID_OUT_BASE, BUTTON_INPUT_SHIFT_DOWN);
}

/* Call back function after 200ms shift up or down */
alt_u32 solenoid_callback(void* context){
	printf("cancel 200ms shift alarm\n");
	IOWR_ALTERA_AVALON_PIO_DATA(SOLENOID_OUT_BASE, CLEAR_BUTTON_DATA);
	//signal_exit_shift_matching();
	return 0;
}

BOOL is_clutchless_shifting_enabled(){
	if (((*(INT8U*) SWITCH_BASE) & SWITCH_ENABLE_SHIFT_MATCHING_MASK) == SWITCH_ENABLE_SHIFT_MATCHING)
		return TRUE;
	else
		return FALSE;
}

/*  Task routine for solenoid */
void solenoid_task(void* pdata) {

	INT8U err;
	btn_input_q = OSQCreate((void*)solenoid_btn_buf, SOLENOID_Q_SIZE_ELEMENTS);
	solenoid_external_failure_flag = OSSemCreate(SEM_FLAG_NO_ERROR);
	solenoid_failure_resolved_flag = OSSemCreate(SEM_FLAG_ERROR_UNRESOLVED);
	rpm_matching_result_q = OSQCreate((void*)shift_matching_result_buf, SOLENOID_Q_SIZE_ELEMENTS);
	OS_EVENT *shift_matching_q = NULL;
	INT8U curr_gear = 1;
	output_curr_gear(curr_gear);
	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(BUTTONS_BASE, BUTTON_INPUT_SHIFT_UP | BUTTON_INPUT_SHIFT_DOWN);
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(BUTTONS_BASE, CLEAR_BUTTON_EDGE_REG);
	alt_irq_register(BUTTONS_IRQ, NULL,  &isr_btn);
	alt_alarm* shift_alarm = (alt_alarm*)malloc(sizeof(alt_alarm));
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
		OSQFlush(btn_input_q);
		IOWR_ALTERA_AVALON_PIO_EDGE_CAP(BUTTONS_BASE, CLEAR_BUTTON_EDGE_REG);
		IOWR_ALTERA_AVALON_PIO_IRQ_MASK(BUTTONS_BASE, BUTTON_INPUT_SHIFT_UP | BUTTON_INPUT_SHIFT_DOWN);
		INT32U shift_command = *(INT32U *) OSQPend(btn_input_q, Q_TIMEOUT_WAIT_FOREVER, &err);
		IOWR_ALTERA_AVALON_PIO_IRQ_MASK(BUTTONS_BASE, 0);
		if(shift_matching_q == NULL)
			shift_matching_q = get_shift_matching_q();
		if(OSSemAccept(solenoid_external_failure_flag) != SEM_FLAG_NO_ERROR){
			printf("External failure, block solenoid task\n");
			OSSemPend(solenoid_failure_resolved_flag, Q_TIMEOUT_WAIT_FOREVER, &err);
		}
		printf("cmd:%lu\n", shift_command);
//		if(is_clutchless_shifting_enabled() == FALSE)
//			continue;
		INT16U new_gear = curr_gear;
		if(shift_command == BUTTON_INPUT_SHIFT_UP){
			if(curr_gear == NUM_GEARS){
				continue;
			}
//			if(get_RPM() < SHIFT_UP_LOWER_BOUND_RPM){
//				printf("Cannot up-shift because RPM is too low\n");
//				continue;
//			}
			new_gear++;
		}else if (shift_command == BUTTON_INPUT_SHIFT_DOWN){
			if(curr_gear == 1){
				continue;
			}
//			if(get_RPM() > SHIFT_DOWN_UPPER_BOUND_RPM){
//				printf("Cannot down-shift because RPM is too high\n");
//				continue;
//			}
			new_gear--;
		}
		shift_req* req = (shift_req*) malloc(sizeof(shift_req));
		req->curr_gear = curr_gear;
		req->new_gear = new_gear;
		printf("putting new gear %d into matching q\n", new_gear);
//		OSQPost(shift_matching_q, (void*)req);
//		INT8U result = (INT8U) OSQPend(rpm_matching_result_q, Q_TIMEOUT_WAIT_FOREVER, &err);
//		if(result == SHIFT_MATCHING_RESULT_FAIL){
//			printf("failure encountered when shift matching\n");
//			continue;
//		}
		curr_gear = new_gear;
		output_curr_gear(curr_gear);
		free(req);
#endif
		if (shift_command == BUTTON_INPUT_SHIFT_UP){
			printf("setting 200ms shift up alarm\n");
			shift_up();
			alt_alarm_start(shift_alarm, SOLENOID_OPEN_DURATION_TICKS, &solenoid_callback, &shift_alarm);
		}
		else if (shift_command == BUTTON_INPUT_SHIFT_DOWN){
			printf("setting 200ms shift down alarm\n");
			shift_down();
			alt_alarm_start(shift_alarm, SOLENOID_OPEN_DURATION_TICKS, &solenoid_callback, &shift_alarm);
		}else {
			printf("Unknown shift command received: %lu\n", shift_command);
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

OS_EVENT* get_solenoid_task_external_failure_flag(){
	return solenoid_external_failure_flag;
}

OS_EVENT* get_solenoid_task_failure_resolved_flag(){
	return solenoid_failure_resolved_flag;
}

OS_EVENT* get_rpm_matching_result_q(){
	return rpm_matching_result_q;
}
