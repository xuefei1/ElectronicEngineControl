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

#include "custom_types.h"
#include "proj_config.h"

#define DEFAULT_GEAR 						2

#define PULSE_WIDTH_TO_DEG_INCREMENT 		1

#define THROTTLE_INC_BY_100_RPM 			221

void throttle_data_init();

INT16U get_tps_from_apps(INT16U apps_reading);

INT16U get_new_RPM_needed(INT16U curr_RPM, INT8U curr_gear, INT8U new_gear);

#endif /* THROTTLE_DATA_H_ */
