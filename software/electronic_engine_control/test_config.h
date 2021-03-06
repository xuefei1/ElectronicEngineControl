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

#include "tests/CuTest.h"

/* Set this flag to enable running all unit tests*/
//#define RUN_UNIT_TESTS

/* Set this flag to get average time required for each task*/
//#define RUN_AVG_TASK_TIME_TEST
#define AVG_ITERATION 						1000

/* Set this flag to run pwm gen test, this test must be run from within a task*/
#define RUN_PWM_TEST_TASK

#define CHANGED_BY_THRESHOLD_INPUT_1		100
#define CHANGED_BY_THRESHOLD_INPUT_2		500
#define CHANGED_BY_THRESHOLD_INPUT_3		150

#define DIFFER_BY_PERCENT_INPUT_1			0
#define DIFFER_BY_PERCENT_INPUT_2			1
#define DIFFER_BY_PERCENT_INPUT_3			10
#define DIFFER_BY_PERCENT_INPUT_4			15
#define DIFFER_BY_PERCENT_INPUT_5			25

#define PWM_TEST_PERIOD_START				1000
#define PWM_TEST_PERIOD_END					60000
#define PWM_TEST_PERIOD_INCREMENT			1000
#define PWM_TEST_DUTY_START					0
#define PWM_TEST_DUTY_END					100
#define PWM_TEST_DUTY_INCREMENT				10
#define PWM_TEST_SLEEP_DURATION_US			5000000

#define TEST_TASK_PRIO						1

#define TEST_PWM(period, duty)	Test_pwm_gen(period, duty)

#define TEST_THROTTLE_OPEN(percent)	Test_throttle_open(percent)

#define TEST_THROTTLE_CLOSE(percent)	Test_throttle_close(percent)

int run_all_unit_tests();
CuSuite* CuGetSuite();
void Test_int16U_differ_by_percent(CuTest* tc);
void Test_int16U_changed_by_threshold(CuTest* tc);
void Test_get_lcd_msg_q(CuTest* tc);
void Test_get_failure_msg_q(CuTest* tc);
void Test_get_new_tps_reading_sem(CuTest* tc);
void Test_get_motor_cmd_q(CuTest* tc);
void Test_get_expected_tps_reading(CuTest* tc);
void Test_set_new_motor_position(CuTest* tc);
static void X_CompareAsserts(CuTest* tc, const char *file, int line, const char* message, const char* expected, CuString *actual);

void Test_pwm_gen_sweep();

void Test_pwm_gen(INT32U period, INT16U duty);

void Test_throttle_open(INT16U percent);

void Test_throttle_close(INT16U percent);

#endif /* TEST_CONFIG_H_ */
