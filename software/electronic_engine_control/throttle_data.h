/*
 * throttle_data.h
 *
 *  Created on: Mar 8, 2017
 *      Author: xuefei1
 *
 *      Status: C
 */

#ifndef THROTTLE_DATA_H_
#define THROTTLE_DATA_H_

#include "proj_config.h"

#define DEFAULT_GEAR 						2

#define PULSE_WIDTH_TO_DEG_INCREMENT 		1

#define THROTTLE_INC_BY_100_RPM			(TPS_VALID_VALUE_FULLY_CLOSED - TPS_VALID_VALUE_FULLY_OPENED) / MAX_SCALED_RPM

void throttle_data_init();

INT16U get_tps_from_throttle_open_deg(INT16U deg);

INT16U get_throttle_open_deg_from_apps(INT16U apps_reading);

INT16U get_throttle_open_deg_from_tps(INT16U tps_reading);

INT16U get_tps_from_apps(INT16U apps_reading);

INT16U get_new_rpm_needed(INT16U curr_rpm, INT8U curr_gear, INT8U new_gear);

INT16U get_duty_cycle_from_throttle_open_deg(INT16U deg);

INT16U get_duty_cycle_from_throttle_close_deg(INT16U deg);

#endif /* THROTTLE_DATA_H_ */
