/*
 * test_config.h
 *
 * Status: C
 *
 *  Created on: Feb 2, 2017
 *      Author: Fred
 *
 *  Header containing global test bench config
 *
 *  *See proj_config.h for acronym translations
 */

#ifndef TEST_CONFIG_H_
#define TEST_CONFIG_H_

#include "proj_config.h"
#include "apps_motor_proc.h"
#include "failure_handler_proc.h"
#include "tps_proc.h"
#include "ui_proc.h"
#include "util.h"

#define CHANGED_BY_THRESHOLD_INPUT_1		100
#define CHANGED_BY_THRESHOLD_INPUT_2		500
#define CHANGED_BY_THRESHOLD_INPUT_3		150

#define DIFFER_BY_PERCENT_INPUT_1			0
#define DIFFER_BY_PERCENT_INPUT_2			1
#define DIFFER_BY_PERCENT_INPUT_3			10
#define DIFFER_BY_PERCENT_INPUT_4			15
#define DIFFER_BY_PERCENT_INPUT_5			25

/* Set this flag to enable running all unit tests*/
#define RUN_ALL_TESTS

int run_all_tests();
CuSuite* CuGetSuite(void);
void Test_int16U_differ_by_percent(CuTest* tc);
void Test_int16U_changed_by_threshold(CuTest* tc);
void Test_get_lcd_msg_q(CuTest* tc);
void Test_get_failure_msg_q(CuTest* tc);
void Test_get_expected_motor_pos_q(CuTest* tc);
void Test_get_new_tps_reading_sem(CuTest* tc);
void Test_get_motor_cmd_q(CuTest* tc);
void Test_get_expected_tps_reading(CuTest* tc);
void Test_set_new_motor_position(CuTest* tc);
static void X_CompareAsserts(CuTest* tc, const char *file, int line, const char* message, const char* expected, CuString *actual)

#endif /* TEST_CONFIG_H_ */
