/*
 * tps_proc.h
 *
 * Status: C
 *
 *  Created on: Feb 4, 2017
 *      Author: xuefei1
 *
 *      Header for Throttle Position Sensor process
 */

#ifndef TPS_PROC_H_
#define TPS_PROC_H_

#include "proj_config.h"
#include "util.h"

#define TPS_TASK_PRIORITY      				3

/* ADC channel assignments */
#define TPS_1_ADC_CHANNEL			  		2
#define TPS_2_ADC_CHANNEL			  		3

/* Task Delays*/
#define TPS_TASK_DELAY_HOURS	  			0
#define TPS_TASK_DELAY_MINUTES	  			0
#define TPS_TASK_DELAY_SECONDS	  			0
#define TPS_TASK_DELAY_MILLISEC	  			50

/* detect if TPS expected and actual value differ*/
#define TPS_VALUE_DIFFER_FROM_EXPECTED(input, exp)	int16U_changed_by_threshold(input, exp, TPS_VALUE_TOLERANCE)

/* detect if TPS readings differ by a percentage */
#define TPS_VALUE_MISMATCH(input1, input2)			int16U_differ_by_percent(input1, input2, TPS_VALUE_DIFFERENCE_PERCENT, PERCENT_DIFF_ACCURACY)


void tps_task(void* pdata);

#endif /* TPS_PROC_H_ */
