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
//value at index i represents the duty cycle needed for the 20 kHz signal to open the throttle
//throttle resolution could be less than 1 deg, in that case, many degress could have the same
//duty cycle value
//remember this duty cycle only controls opening of throttle plate
//to close the plate, we need a pwm signal with different polarity and a feedback system is used
INT16U throttle_deg_open_pwm_duty_cycle[MAX_THROTTLE_DEG + 1];

void throttle_data_init(){
	INT16U apps_val_inc_by_1_deg = (APPS_VALID_VALUE_FULLY_PRESSED - APPS_VALID_VALUE_FULLY_RELEASED) / MAX_THROTTLE_DEG;
	INT16U tps_val_inc_by_1_deg = (TPS_VALID_VALUE_FULLY_OPENED - TPS_VALID_VALUE_FULLY_CLOSED) / MAX_THROTTLE_DEG;
	INT16U deg_inc_by_1_percent_duty = MAX_THROTTLE_DEG / (MOTOR_PWM_DUTY_CYCLE_FULLY_CLOSE - MOTOR_PWM_DUTY_CYCLE_FULLY_OPEN) + 1;
	INT16U i;
	throttle_deg_apps_val_map[0] = APPS_VALID_VALUE_FULLY_RELEASED;
	throttle_deg_tps_val_map[0] = TPS_VALID_VALUE_FULLY_CLOSED;
	throttle_deg_open_pwm_duty_cycle[0] = MOTOR_PWM_DUTY_CYCLE_FULLY_CLOSE;
	for(i=1; i<MAX_THROTTLE_DEG; i++){
		throttle_deg_apps_val_map[i] = throttle_deg_apps_val_map[i - 1] + apps_val_inc_by_1_deg;
		throttle_deg_tps_val_map[i] = throttle_deg_tps_val_map[i - 1] + tps_val_inc_by_1_deg;
		if(i % deg_inc_by_1_percent_duty == 0){
			throttle_deg_open_pwm_duty_cycle[i] = throttle_deg_open_pwm_duty_cycle[i-1] - 1; // decrease duty cycle
		}else{
			throttle_deg_open_pwm_duty_cycle[i] = throttle_deg_open_pwm_duty_cycle[i-1];
		}
	}
	throttle_deg_apps_val_map[MAX_THROTTLE_DEG] = APPS_VALID_VALUE_FULLY_PRESSED;
	throttle_deg_tps_val_map[MAX_THROTTLE_DEG] = TPS_VALID_VALUE_FULLY_OPENED;
	throttle_deg_open_pwm_duty_cycle[MAX_THROTTLE_DEG] = MOTOR_PWM_DUTY_CYCLE_FULLY_OPEN;
}

INT16U get_throttle_open_deg_from_apps(INT16U apps_reading){
	if(apps_reading >= APPS_VALID_VALUE_FULLY_PRESSED){
			return MAX_THROTTLE_DEG;
	}
	if(apps_reading <= APPS_VALID_VALUE_FULLY_RELEASED){
		return MIN_THROTTLE_DEG;
	}
	INT8U upper;
	for(upper = 1; upper < MAX_THROTTLE_DEG; upper++){
		if(apps_reading <= throttle_deg_apps_val_map[upper] && apps_reading > throttle_deg_apps_val_map[upper - 1]){
			return upper;
		}
	}
	return MAX_THROTTLE_DEG;
}

INT16U get_tps_from_apps(INT16U apps_reading){
	if(apps_reading >= APPS_VALID_VALUE_FULLY_PRESSED){
			return throttle_deg_tps_val_map[MAX_THROTTLE_DEG];
	}
	if(apps_reading <= APPS_VALID_VALUE_FULLY_RELEASED){
		return throttle_deg_tps_val_map[MIN_THROTTLE_DEG];
	}
	INT8U upper;
	for(upper = 1; upper < MAX_THROTTLE_DEG; upper++){
		if(apps_reading <= throttle_deg_apps_val_map[upper] && apps_reading > throttle_deg_apps_val_map[upper - 1]){
			return throttle_deg_tps_val_map[upper];
		}
	}
	return throttle_deg_tps_val_map[MAX_THROTTLE_DEG];
}

INT16U get_throttle_open_deg_from_tps(INT16U tps_reading){
	if(tps_reading >= TPS_VALID_VALUE_FULLY_OPENED){
			return MAX_THROTTLE_DEG;
	}
	if(tps_reading <= TPS_VALID_VALUE_FULLY_CLOSED){
		return MIN_THROTTLE_DEG;
	}
	INT8U upper;
	for(upper = 1; upper < MAX_THROTTLE_DEG; upper++){
		if(tps_reading <= throttle_deg_tps_val_map[upper] && tps_reading > throttle_deg_tps_val_map[upper - 1]){
			return upper;
		}
	}
	return MAX_THROTTLE_DEG;
}

INT16U get_new_rpm_needed(INT16U curr_rpm, INT8U curr_gear, INT8U new_gear){
	INT8U curr_gear_ratio = get_gear_ratio(curr_gear);
	INT8U new_gear_ratio = get_gear_ratio(new_gear);
	return ((INT16U)curr_rpm * new_gear_ratio / curr_gear_ratio)/FLOAT_SCALE_FACTOR_10;
}

//input param: the degree you want the throttle to OPEN by
INT16U get_duty_cycle_from_throttle_open_deg(INT16U deg){
	if(deg > MAX_THROTTLE_DEG){
		deg = MAX_THROTTLE_DEG;
	}
	return throttle_deg_open_pwm_duty_cycle[deg];
}

INT16U get_duty_cycle_from_throttle_close_deg(INT16U deg){
	return throttle_deg_open_pwm_duty_cycle[MOTOR_PWM_DUTY_CYCLE_FULLY_OPEN];
}

INT16U get_tps_from_throttle_open_deg(INT16U deg){
	if(deg > MAX_THROTTLE_DEG){
			deg = MAX_THROTTLE_DEG;
	}
	return throttle_deg_tps_val_map[deg];
}
