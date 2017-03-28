/*
 * eng_trans_data.h
 *
 *  Created on: Mar 26, 2017
 *      Author: xuefei1
 *
 *  Defines data/functions related to engine/transmission
 */

#ifndef ENG_TRANS_DATA_H_
#define ENG_TRANS_DATA_H_

#include "proj_config.h"

#define ENGINE_SOUND_PERIOD_TICKS			150000

#define MAX_RPM								8000

#define MIN_RPM								500

#define SHIFT_UP_LOWER_BOUND_RPM			1000

#define SHIFT_DOWN_UPPER_BOUND_RPM			6000

#define RPM_SCALE_FACTOR					3

#define RPM_ADC_CHANNEL			  			6

#define NUM_GEARS							4

#define GEAR_RATIO_1ST						18

#define GEAR_RATIO_2ND						12

#define GEAR_RATIO_3RD						10

#define GEAR_RATIO_4TH						8

INT8U get_gear_ratio(INT8U gear);

void output_curr_gear(INT8U gear);

INT16U get_RPM();

void generate_engine_sound(INT16U apps_val);

#endif /* ENG_TRANS_DATA_H_ */
