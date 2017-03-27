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
	INT16U rpm = (INT16U) alt_up_de0_nano_adc_read(get_adc(), RPM_ADC_CHANNEL);
	rpm *= RPM_SCALE_FACTOR;
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
