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
#include "pwm_gen.h"

#define ENGINE_SOUND_PERIOD_TICKS			150000

#define MAX_RPM								10500

#define MIN_RPM								3500

#define SHIFT_UP_LOWER_BOUND_RPM			4500

#define SHIFT_DOWN_UPPER_BOUND_RPM			7000

#define OS_SEM_RPM_NOT_REACHED				0

#define RPM_SCALE_FACTOR					3

#define RPM_ADC_CHANNEL			  			6

#define NUM_GEARS							4

#define GEAR_RATIO_1ST						18

#define GEAR_RATIO_2ND						13

#define GEAR_RATIO_3RD						10

#define GEAR_RATIO_4TH						8

INT8U get_gear_ratio(INT8U gear);

void output_curr_gear(INT8U gear);

INT16U get_RPM();

void generate_engine_sound(pwm_gen_module* module, INT16U deg);

#endif /* ENG_TRANS_DATA_H_ */
