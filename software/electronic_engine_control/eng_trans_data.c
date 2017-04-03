/*
 * eng_trans_data.c
 *
 * Status: E
 *
 *  Created on: Mar 26, 2017
 *      Author: xuefei1
 *
 *	Contains functions related to engine/transmission
 */

#include "eng_trans_data.h"

INT8U get_gear_ratio(INT8U gear){
	if(gear == 1){
		return GEAR_RATIO_1ST;
	}else if(gear == 2){
		return GEAR_RATIO_2ND;
	}else if(gear == 4){
		return GEAR_RATIO_4TH;
	}else{
		return GEAR_RATIO_3RD;
	}
}

INT16U get_RPM(){
	INT32U duty_count = *(INT32U*) RPM_PWM_DECODER_BASE;
	INT16U rpm = (duty_count - MIN_RPM_DUTY_COUNT) / DUTY_COUNT_TO_RPM_FACTOR + MIN_RPM;
	if(rpm > MAX_RPM)
		rpm = MAX_RPM;
	if(rpm < MIN_RPM)
		rpm = MIN_RPM;
	return rpm;
}

void output_curr_gear(INT8U gear){
	if(gear == 1){
		*(INT8U*)CURR_GEAR_OUT_BASE = 0;
	}else if(gear == 2){
		*(INT8U*)CURR_GEAR_OUT_BASE = 1;
	}else if(gear == 4){
		*(INT8U*)CURR_GEAR_OUT_BASE = 3;
	}else{
		*(INT8U*)CURR_GEAR_OUT_BASE = 2;
	}
}

INT16U get_new_rpm_needed(INT16U curr_rpm, INT8U curr_gear, INT8U new_gear){
	INT8U curr_gear_ratio = get_gear_ratio(curr_gear);
	INT8U new_gear_ratio = get_gear_ratio(new_gear);
	INT16U rpm = ((INT16U)curr_rpm * new_gear_ratio / curr_gear_ratio);
	if(rpm > MAX_RPM)
		rpm = MAX_RPM;
	if(rpm < MIN_RPM)
		rpm = MIN_RPM;
	return rpm;
}

void generate_engine_sound(pwm_gen_module* module, INT16U deg){
	deg *= 10;
	set_duty_cycle(module, deg + PWM_DUTY_CYCLE_HALF > PWM_DUTY_CYCLE_HIGH ?
			PWM_DUTY_CYCLE_HIGH : deg + PWM_DUTY_CYCLE_HALF);
}
