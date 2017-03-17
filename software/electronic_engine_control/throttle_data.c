/*
 * throttle_data.c
 *
 *  Created on: Mar 8, 2017
 *      Author: xuefei1
 *
 *      Status: C
 */

#include "throttle_data.h"
//index 0 to 90, representing 0 to 90 degree of throttle plate travel
//the value at index i and value at index i+1 indicates if APPS value is within this range
//then the throttle plate will be at i+1 degrees
//if apps reading value == value at index i, then throttle plate is at i degrees
INT16U throttle_deg_apps_val_map[MAX_THROTTLE_DEG + 1];
//same idea as above
INT16U throttle_deg_tps_val_map[MAX_THROTTLE_DEG + 1];

void throttle_data_init(){
	INT16U apps_val_inc_by_1_deg = (APPS_VALID_VALUE_MAX - APPS_VALID_VALUE_MIN) / MAX_THROTTLE_DEG; 
	INT16U tps_val_inc_by_1_deg = (TPS_VALID_VALUE_MAX - TPS_VALID_VALUE_MIN) / MAX_THROTTLE_DEG; 
	INT8U i;
	apps_val_inc_by_1_deg[0] = APPS_VALID_VALUE_MIN;
	tps_val_inc_by_1_deg[0] = TPS_VALID_VALUE_MIN;
	for(i=1; i<MAX_THROTTLE_DEG; i++){
		throttle_deg_apps_val_map[i] = throttle_deg_apps_val_map[i - 1] + apps_val_inc_by_1_deg;
		throttle_deg_tps_val_map[i] = throttle_deg_tps_val_map[i - 1] + tps_val_inc_by_1_deg;
	}
	throttle_deg_apps_val_map[MAX_THROTTLE_DEG] = APPS_VALID_VALUE_MAX;
	throttle_deg_tps_val_map[MAX_THROTTLE_DEG] = TPS_VALID_VALUE_MAX;
}

INT16U get_tps_from_apps(INT16U apps_reading){
	if(apps_reading >= APPS_VALID_VALUE_MAX){
		return (throttle_deg_tps_val_map[MAX_THROTTLE_DEG] + throttle_deg_tps_val_map[MAX_THROTTLE_DEG - 1]) / 2;
	}
	if(apps_reading <= APPS_VALID_VALUE_MIN){
		return throttle_deg_tps_val_map[0];
	}
	INT8U upper;
	for(upper = 1; upper < MAX_THROTTLE_DEG; upper++){
		if(apps_reading <= throttle_deg_apps_val_map[upper] && apps_reading > throttle_deg_apps_val_map[upper - 1]){
			return throttle_deg_tps_val_map[upper];
		}
	}
	return (throttle_deg_tps_val_map[MAX_THROTTLE_DEG] + throttle_deg_tps_val_map[MAX_THROTTLE_DEG - 1]) / 2;
}

INT16U get_new_RPM_needed(INT16U curr_RPM, INT8U curr_gear, INT8U new_gear){
	INT8U curr_gear_ratio = get_gear_ratio(curr_gear);
	INT8U new_gear_ratio = get_gear_ratio(new_gear);
	return ((INT16U)curr_RPM * new_gear_ratio / curr_gear_ratio)/FLOAT_SCALE_FACTOR_10;
}


