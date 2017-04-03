/*
 *
 *
 * Status: E
 *
 *  Created on: Feb 2, 2017
 *      Author: Fred
 *
 */
#include "throttle_control_task.h"
/* Used to accept possible commands from other modules */
OS_EVENT *shift_matching_q;
/* Flag indicating possible APPS failure */
OS_EVENT *apps_failure_flag;
/* Flag indicating possible TPS failure */
OS_EVENT *tps_failure_flag;
/* Flag indicating possible motor failure */
OS_EVENT *motor_failure_flag;
/* Flag indicating failures detected in other tasks */
OS_EVENT *throttle_task_external_failure_flag;
/* Flag indicating failure resolved */
OS_EVENT *throttle_task_failure_resolved_flag;
OS_EVENT *failure_msg_q;
OS_EVENT *exit_shift_matching_flag;
OS_EVENT *matching_result_q;
INT16U shift_cmd_q_buf[MOTOR_CMD_Q_SIZE_ELEMENTS];
alt_up_de0_nano_adc_dev* adc;

void signal_shift_start(OS_EVENT* rpm_matching_result_q){
	OSQPost(rpm_matching_result_q, (void*) SHIFT_MATCHING_RESULT_OK);
}

void signal_shift_failure(OS_EVENT* rpm_matching_result_q){
	OSQPost(rpm_matching_result_q, (void*) SHIFT_MATCHING_RESULT_FAIL);
}

void set_throttle_by_duty_cycle(pwm_gen_module* pwm_throttle_open, pwm_gen_module* pwm_throttle_close, INT16U duty_cycle){
	set_duty_cycle(pwm_throttle_open, duty_cycle);
	set_duty_cycle(pwm_throttle_close, PWM_DUTY_CYCLE_HIGH);
}

void increase_throttle_by_feedback(pwm_gen_module* pwm_throttle_open, pwm_gen_module* pwm_throttle_close, INT16U* curr_duty_cycle, INT16U resolution){
	INT16U new_duty_cycle = *curr_duty_cycle - resolution;
	if(new_duty_cycle > MOTOR_PWM_DUTY_CYCLE_FULLY_CLOSE)
		new_duty_cycle = MOTOR_PWM_DUTY_CYCLE_FULLY_CLOSE;
	if(new_duty_cycle < MOTOR_PWM_DUTY_CYCLE_FULLY_OPEN)
		new_duty_cycle = MOTOR_PWM_DUTY_CYCLE_FULLY_OPEN;
	set_throttle_by_duty_cycle(pwm_throttle_open, pwm_throttle_close, new_duty_cycle);
	*curr_duty_cycle = new_duty_cycle;
}

void decrease_throttle(pwm_gen_module* pwm_throttle_open, pwm_gen_module* pwm_throttle_close){
	set_duty_cycle(pwm_throttle_close, PWM_DUTY_CYCLE_HIGH);
	set_duty_cycle(pwm_throttle_open, PWM_DUTY_CYCLE_HIGH);
}

alt_u32 watchdog_throttle_position_callback(void* context){
	alt_alarm* alarm  = (alt_alarm*)context;
	clean_alarm(&alarm);
	if(OSSemAccept(motor_failure_flag) == SEM_FLAG_NO_ERROR) OSSemPost(motor_failure_flag);
	OSQPost(failure_msg_q, (void*) ERR_EXPECTED_THROTTLE_POS_MISMATCH);
	printf("watchdog throttle position timeout\n");
	return 0;
}

alt_u32 watchdog_rpm_matching_callback(void* context){
	alt_alarm* alarm  = (alt_alarm*)context;
	clean_alarm(&alarm);
	signal_shift_failure(matching_result_q);
	printf("watchdog rpm matching timeout\n");
	return 0;
}

/* Callback function after 100 ms position sensor comparison timeout */
alt_u32 apps_value_comp_callback(void* context){
	alt_up_de0_nano_adc_update(adc);
	INT16U apps_1_reading = alt_up_de0_nano_adc_read(adc,
			APPS_1_ADC_CHANNEL);
	INT16U apps_2_reading = alt_up_de0_nano_adc_read(adc,
			APPS_2_ADC_CHANNEL);
	alt_alarm* alarm  = (alt_alarm*)context;
	clean_alarm(&alarm);
	if (APPS_VALUE_MISMATCH(apps_1_reading, apps_2_reading)) {
		if(OSSemAccept(apps_failure_flag) == SEM_FLAG_NO_ERROR)	OSSemPost(apps_failure_flag);
		OSQPost(failure_msg_q, (void*) ERR_APPS_READING_MISMATCH);
	}
	return 0;
}

/* Callback function to check TPS agreement position */
alt_u32 tps_value_comp_callback(void* context){
	alt_up_de0_nano_adc_update(adc);
	INT32U tps_1_reading = alt_up_de0_nano_adc_read(adc, TPS_1_ADC_CHANNEL);
	INT32U tps_2_reading = alt_up_de0_nano_adc_read(adc, TPS_2_ADC_CHANNEL);
	alt_alarm* alarm  = (alt_alarm*)context;
	clean_alarm(&alarm);
	if (TPS_VALUE_MISMATCH(tps_1_reading, tps_2_reading)) {
		if(OSSemAccept(tps_failure_flag) == SEM_FLAG_NO_ERROR)	OSSemPost(tps_failure_flag);
		OSQPost(failure_msg_q, (void*) ERR_TPS_READING_MISMATCH);
	}
	return 0;
}

void check_tps_values(INT16U tps_1_reading, INT16U tps_2_reading){
	static alt_alarm* tps_check_alarm = NULL;
	if (TPS_VALUE_MISMATCH(tps_1_reading, tps_2_reading)) {
		if(tps_check_alarm == NULL){
			tps_check_alarm = (alt_alarm*) malloc(sizeof(alt_alarm));
			alt_alarm_start(tps_check_alarm, SENSOR_VAL_COMP_DELAY_TICKS, &tps_value_comp_callback, tps_check_alarm);
		}
	}else{
		clean_alarm(&tps_check_alarm);
	}
}

void check_apps_values(INT16U apps_1_reading, INT16U apps_2_reading){
	static alt_alarm* apps_check_alarm = NULL;
	if (APPS_VALUE_MISMATCH(apps_1_reading, apps_2_reading)) {
		if(apps_check_alarm == NULL){
			apps_check_alarm = (alt_alarm*) malloc(sizeof(alt_alarm));
			alt_alarm_start(apps_check_alarm, SENSOR_VAL_COMP_DELAY_TICKS, &apps_value_comp_callback, apps_check_alarm);
		}
	}else{
		clean_alarm(&apps_check_alarm);
	}
}

BOOL set_new_throttle_position_by_tps(pwm_gen_module* pwm_throttle_open, pwm_gen_module* pwm_throttle_close,
		INT16U* curr_duty_cycle, INT16U expected_tps, INT16U curr_tps) {
	if(FALSE == TPS_VALUE_DIFFER_FROM_EXPECTED(curr_tps, expected_tps)){
		set_throttle_by_duty_cycle(pwm_throttle_open, pwm_throttle_close, MOTOR_PWM_DUTY_CYCLE_FULLY_CLOSE);
		*curr_duty_cycle = MOTOR_PWM_DUTY_CYCLE_FULLY_CLOSE;
		return TRUE;
	}else if(curr_tps > expected_tps){
		decrease_throttle(pwm_throttle_open, pwm_throttle_close);
	}else{
		increase_throttle_by_feedback(pwm_throttle_open, pwm_throttle_close, curr_duty_cycle, MOTOR_PWM_DUTY_CYCLE_RESOLUTION);
	}
	return FALSE;
}

BOOL set_new_throttle_position_by_rpm(pwm_gen_module* pwm_throttle_open, pwm_gen_module* pwm_throttle_close,
		INT16U* curr_duty_cycle, INT16U expected_rpm, INT16U curr_rpm, INT32U* delay_counter) {
	if(FALSE == RPM_VALUE_DIFFER_FROM_EXPECTED(curr_rpm, expected_rpm)){
		set_throttle_by_duty_cycle(pwm_throttle_open, pwm_throttle_close, MOTOR_PWM_DUTY_CYCLE_FULLY_CLOSE);
		*curr_duty_cycle = MOTOR_PWM_DUTY_CYCLE_FULLY_CLOSE;
		return TRUE;
	}else if(curr_rpm > expected_rpm){
		if(RPM_MATCHING_DELAY_COUNT == *delay_counter){
			decrease_throttle(pwm_throttle_open, pwm_throttle_close);
			*delay_counter = 0;
		}else{
			(*delay_counter)++;
		}
	}else{
		if(RPM_MATCHING_DELAY_COUNT == *delay_counter){
			increase_throttle_by_feedback(pwm_throttle_open, pwm_throttle_close, curr_duty_cycle, MOTOR_PWM_DUTY_CYCLE_RESOLUTION_HIGH);
			*delay_counter = 0;
		}else{
			(*delay_counter)++;
		}
	}
	return FALSE;
}

void reset_failure_flags(){
	while(OSSemAccept(tps_failure_flag) != SEM_FLAG_NO_ERROR);
	while(OSSemAccept(apps_failure_flag) != SEM_FLAG_NO_ERROR);
	while(OSSemAccept(motor_failure_flag) != SEM_FLAG_NO_ERROR);
	while(OSSemAccept(throttle_task_external_failure_flag) != SEM_FLAG_NO_ERROR);
}

BOOL is_slip_control_enabled(){
	if (((*(INT8U*) SWITCH_BASE) & SWITCH_ENABLE_SLIP_CONTROL_MASK) == SWITCH_ENABLE_SLIP_CONTROL)
		return TRUE;
	else
		return FALSE;
}

BOOL is_demo_slip_control_enabled(){
	if (((*(INT8U*) SWITCH_BASE) & SWITCH_ENABLE_SLIP_CONTROL_DEMO_MASK) == SWITCH_ENABLE_SLIP_CONTROL_DEMO)
		return TRUE;
	else
		return FALSE;
}

/*  Task routine for pedal position sensor and motor */
void throttle_control_task(void* pdata) {

	adc = get_adc();
	INT8U err;
	failure_msg_q = get_failure_msg_q();
	matching_result_q = get_rpm_matching_result_q();
	apps_failure_flag = OSSemCreate(SEM_FLAG_NO_ERROR);
	tps_failure_flag = OSSemCreate(SEM_FLAG_NO_ERROR);
	motor_failure_flag = OSSemCreate(SEM_FLAG_NO_ERROR);
	throttle_task_external_failure_flag = OSSemCreate(SEM_FLAG_NO_ERROR);
	throttle_task_failure_resolved_flag = OSSemCreate(SEM_FLAG_ERROR_UNRESOLVED);
	exit_shift_matching_flag = OSSemCreate(SHIFT_MATCHING_IN_PROGRESS);
	alt_alarm* watchdog_throttle_position = NULL;
	alt_alarm* watchdog_rpm_matching = NULL;
	INT32U expected_rpm = 0;
	INT16U expected_tps_reading = 0;
	BOOL slip_control_mode = FALSE;
	INT16U last_apps_1_reading = 0;
	INT32U rpm_matching_delay_counter = 0;
	shift_matching_q = OSQCreate((void*) shift_cmd_q_buf, MOTOR_CMD_Q_SIZE_ELEMENTS);
	if (shift_matching_q == NULL) {
		printf("failed to init q\n");
		return;
	}
	pwm_gen_module* pwm_engine_sound = get_new_pwm_module(PWM_GENERATOR_ENGINE_SOUND_OUT_AVALON_SLAVE_PERIOD_BASE,
		PWM_GENERATOR_ENGINE_SOUND_OUT_AVALON_SLAVE_DUTY_BASE,
		PWM_GENERATOR_ENGINE_SOUND_OUT_AVALON_SLAVE_CONTROL_BASE,
		ENGINE_SOUND_PERIOD_TICKS,
		PWM_DUTY_CYCLE_HALF);
	pwm_gen_module* pwm_throttle_open = get_new_pwm_module(PWM_GENERATOR_THROTTLE_OPEN_AVALON_SLAVE_PERIOD_BASE,
		PWM_GENERATOR_THROTTLE_OPEN_AVALON_SLAVE_DUTY_BASE,
		PWM_GENERATOR_THROTTLE_OPEN_AVALON_SLAVE_CONTROL_BASE,
		MOTOR_PWM_PERIOD_TICKS,
		MOTOR_PWM_DUTY_CYCLE_FULLY_CLOSE);
	pwm_gen_module* pwm_throttle_close = get_new_pwm_module(PWM_GENERATOR_THROTTLE_CLOSE_AVALON_SLAVE_PERIOD_BASE,
		PWM_GENERATOR_THROTTLE_CLOSE_AVALON_SLAVE_DUTY_BASE,
		PWM_GENERATOR_THROTTLE_CLOSE_AVALON_SLAVE_CONTROL_BASE,
		MOTOR_PWM_PERIOD_TICKS,
		PWM_DUTY_CYCLE_HIGH);
	INT16U curr_duty_cycle = MOTOR_PWM_DUTY_CYCLE_FULLY_CLOSE;
	//while(1);
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

	while (1) {
#if !defined(RUN_AVG_TASK_TIME_TEST)
		if(OSSemAccept(motor_failure_flag) != SEM_FLAG_NO_ERROR){
			printf("Possible motor failure, block throttle_control_task\n");
			decrease_throttle(pwm_throttle_open, pwm_throttle_close);
			OSSemPend(throttle_task_failure_resolved_flag, Q_TIMEOUT_WAIT_FOREVER, &err);
			reset_failure_flags();
		}else if(OSSemAccept(apps_failure_flag) != SEM_FLAG_NO_ERROR){
			printf("Possible APPS failure, block throttle_control_task\n");
			decrease_throttle(pwm_throttle_open, pwm_throttle_close);
			OSSemPend(throttle_task_failure_resolved_flag, Q_TIMEOUT_WAIT_FOREVER, &err);
			reset_failure_flags();
		}else if(OSSemAccept(tps_failure_flag) != SEM_FLAG_NO_ERROR){
			printf("Possible TPS failure, block throttle_control_task\n");
			decrease_throttle(pwm_throttle_open, pwm_throttle_close);
			OSSemPend(throttle_task_failure_resolved_flag, Q_TIMEOUT_WAIT_FOREVER, &err);
			reset_failure_flags();
		}else if(OSSemAccept(throttle_task_external_failure_flag) != SEM_FLAG_NO_ERROR){
			printf("External failure, block throttle_control_task\n");
			decrease_throttle(pwm_throttle_open, pwm_throttle_close);
			OSSemPend(throttle_task_failure_resolved_flag, Q_TIMEOUT_WAIT_FOREVER, &err);
			reset_failure_flags();
		}
#endif
		alt_up_de0_nano_adc_update(adc);
		INT16U tps_1_reading = alt_up_de0_nano_adc_read(adc, TPS_1_ADC_CHANNEL);
		INT16U tps_2_reading = alt_up_de0_nano_adc_read(adc, TPS_2_ADC_CHANNEL);
		//printf("rpm raw: %d\n", get_RPM());
		check_tps_values(tps_1_reading, tps_2_reading);
		INT16U curr_tps_reading = tps_1_reading;
		generate_engine_sound(pwm_engine_sound, get_throttle_open_deg_from_tps(curr_tps_reading));

		//Shift matching active
		alt_up_de0_nano_adc_update(adc);
		if(watchdog_rpm_matching != NULL){
			if(FALSE == set_new_throttle_position_by_rpm(pwm_throttle_open, pwm_throttle_close,
					&curr_duty_cycle, expected_rpm, get_RPM(), &rpm_matching_delay_counter)){
				continue;
			}else{
				printf("target rpm reached\n");
				rpm_matching_delay_counter = 0;
				clean_alarm(&watchdog_rpm_matching);
				signal_shift_start(matching_result_q);
				OSSemPend(exit_shift_matching_flag, Q_TIMEOUT_WAIT_FOREVER, &err);
			}
		}
		//WSS checking
		alt_up_de0_nano_adc_update(adc);
		INT16U wheel_speed_left = alt_up_de0_nano_adc_read(adc, WS_1_ADC_CHANNEL);
		INT16U wheel_speed_right = alt_up_de0_nano_adc_read(adc, WS_2_ADC_CHANNEL);
		if(is_demo_slip_control_enabled() == TRUE){
			wheel_speed_right ++;
			wheel_speed_right *= 2;
		}
		if(WS_VALUE_MISMATCH(wheel_speed_left, wheel_speed_right) && is_slip_control_enabled() == TRUE){
			if(slip_control_mode == TRUE) continue;
			clean_alarm(&watchdog_throttle_position);
			*(INT8U*)GREEN_LEDS_BASE = WSS_ACTIVE_LED;
			slip_control_mode = TRUE;
			set_new_throttle_position_by_tps(pwm_throttle_open, pwm_throttle_close,
					&curr_duty_cycle, SLIP_CONTROL_THROTTLE_POS, curr_tps_reading);
			continue;
		}else{
			slip_control_mode = FALSE;
			*(INT8U*)GREEN_LEDS_BASE = NO_ERROR_INDICATION_LED;
		}

		//Shift matching checking
		shift_req* ptr = NULL;
		if(watchdog_rpm_matching == NULL){
			ptr = (shift_req*) OSQAccept(shift_matching_q, &err);
			if(ptr != NULL){
				printf("start shift matching\n");
				expected_rpm = get_new_rpm_needed(get_RPM(), ptr->curr_gear, ptr->new_gear);
				clean_alarm(&watchdog_rpm_matching);
				watchdog_rpm_matching = (alt_alarm*) malloc(sizeof(alt_alarm));
				alt_alarm_start(watchdog_rpm_matching, MOTOR_RPM_DRIVE_DELAY_TICKS, &watchdog_rpm_matching_callback, (void*) watchdog_rpm_matching);
				continue;
			}
		}

		//APPS checking
		alt_up_de0_nano_adc_update(adc);
		INT16U apps_1_reading = alt_up_de0_nano_adc_read(adc,
				APPS_1_ADC_CHANNEL);
		INT16U apps_2_reading = alt_up_de0_nano_adc_read(adc,
				APPS_2_ADC_CHANNEL);
		if (APPS_VALUE_CHANGED(apps_1_reading, last_apps_1_reading)) {
			last_apps_1_reading = apps_1_reading;
			clean_alarm(&watchdog_throttle_position);
			check_apps_values(apps_1_reading, apps_2_reading);
			expected_tps_reading = get_tps_from_apps(apps_1_reading);
			watchdog_throttle_position = (alt_alarm*) malloc(sizeof(alt_alarm));
//			printf("apps1 read value:%d\n", apps_1_reading);
//			printf("apps2 read value:%d\n", apps_2_reading);
			alt_alarm_start(watchdog_throttle_position, MOTOR_DRIVE_DELAY_TICKS, &watchdog_throttle_position_callback, (void*) watchdog_throttle_position);
		}
		tps_1_reading = alt_up_de0_nano_adc_read(adc, TPS_1_ADC_CHANNEL);
		tps_2_reading = alt_up_de0_nano_adc_read(adc, TPS_2_ADC_CHANNEL);
//		printf("tps1 read value:%d\n", tps_1_reading);
//		printf("tps2 read value:%d\n", tps_2_reading);
//		printf("curr duty:%d\n",curr_duty_cycle);
		check_tps_values(tps_1_reading, tps_2_reading);
		if(TRUE == set_new_throttle_position_by_tps(pwm_throttle_open, pwm_throttle_close, &curr_duty_cycle,
				expected_tps_reading, curr_tps_reading)){
			clean_alarm(&watchdog_throttle_position);
		}

#if defined(RUN_AVG_TASK_TIME_TEST)
		iteration_count++;
		if(iteration_count % AVG_ITERATION == 0){
			end_tick = alt_timestamp();
			printf("a%d:%d\n", iteration_count, (end_tick - start_tick)/iteration_count);
		}
#endif
		OSTimeDlyHMSM(THROTTLE_CONTROL_TASK_DELAY_HOURS,
				THROTTLE_CONTROL_TASK_DELAY_MINUTES,
				THROTTLE_CONTROL_TASK_DELAY_SECONDS,
				THROTTLE_CONTROL_TASK_DELAY_MILLISEC);
	}
}

/* Getter for motor command Q for other modules to use */
OS_EVENT* get_shift_matching_q() {
	return shift_matching_q;
}

OS_EVENT* get_throttle_control_task_external_failure_flag(){
	return throttle_task_external_failure_flag;
}

OS_EVENT* get_throttle_control_task_failure_resolved_flag(){
	return throttle_task_failure_resolved_flag;
}

void signal_exit_shift_matching(){
	if(OSSemAccept(exit_shift_matching_flag) == SHIFT_MATCHING_IN_PROGRESS)
		OSSemPost(exit_shift_matching_flag);
}
