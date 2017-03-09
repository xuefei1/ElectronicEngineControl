/*
 * throttle_data.c
 *
 *  Created on: Mar 8, 2017
 *      Author: xuefei1
 *
 *      Status: C
 */

#include "throttle_data.h"

INT8U throttle_angle_to_duty_cycle[MAX_THROTTLE_DEG + 1];

INT16U RPM_gear_to_throttle_pos[MAX_SCALED_RPM + 1][NUM_GEARS + 1];

#define GET_GEAR_RATIO(GEAR)				get_gear_ratio(GEAR)

void throttle_data_init(){
	INT8U i;
	INT8U j;
	throttle_angle_to_duty_cycle[0] = 0;
	for(i = 1; i < MAX_THROTTLE_DEG + 1; i++){
		throttle_angle_to_duty_cycle[i] = throttle_angle_to_duty_cycle[i - 1] + PULSE_WIDTH_TO_DEG_INCREMENT;
	}
	RPM_gear_to_throttle_pos[0][0] = 0;
	RPM_gear_to_throttle_pos[0][1] = 0;
	RPM_gear_to_throttle_pos[0][2] = 0;
	RPM_gear_to_throttle_pos[0][3] = 0;
	RPM_gear_to_throttle_pos[0][4] = 0;
	for(i = 1; i < MAX_SCALED_RPM + 1; i++){
		for(j = 1; j < NUM_GEARS + 1; j++){
			RPM_gear_to_throttle_pos[i][j] = RPM_gear_to_throttle_pos[i - 1][j - 1] + THROTTLE_INC_BY_100_RPM * get_gear_ratio(j) / 10;
		}
	}
}

INT8U get_duty_cycle_for_angle(INT8U angle){
	if(angle > MAX_THROTTLE_DEG){
		angle = MAX_THROTTLE_DEG;
	}
	return throttle_angle_to_duty_cycle[angle];
}

INT16U get_throttle_pos_for_RPM_gear(INT8U gear, INT8U RPM_scaled){
	if(RPM_scaled > MAX_SCALED_RPM){
		RPM_scaled = MAX_SCALED_RPM;
	}
	if(gear > NUM_GEARS){
		gear = DEFAULT_GEAR;
	}
	return RPM_gear_to_throttle_pos[RPM_scaled][gear];
}

INT16U get_new_RPM_needed(INT16U curr_RPM, INT8U curr_gear, INT8U new_gear){
	INT8U curr_gear_ratio = get_gear_ratio(curr_gear);
	INT8U new_gear_ratio = get_gear_ratio(new_gear);
	return ((INT16U)curr_RPM * new_gear_ratio / curr_gear_ratio)/10;
}


