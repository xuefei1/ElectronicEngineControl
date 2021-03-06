/*
 * testbench.c
 *
 * Status: T
 *
 *  Created on: Feb 3, 2017
 *      Author: Fred, Nicholas
 *
 *      Run all the unit tests
 *
 *	Reference: https://github.com/ennorehling/cutest
 *	
 */
#include "../custom_types.h"
#include "../test_config.h"
#include <assert.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../util.h"
#include "../pwm_gen.h"
#include "CuTest.h"

/*-------------------------------------------------------------------------*
 * apps_motor_proc Test
 *-------------------------------------------------------------------------*/
//void Test_get_expected_motor_pos_q(CuTest* tc){
//	OS_EVENT* Curr_expected_tps_reading_q = get_expected_motor_pos_q();
//	CuAssertTrue(tc,Curr_expected_tps_reading_q != NULL);
//	CuAssertIntEquals(tc, Curr_expected_tps_reading_q->OSEventType, expected_tps_reading_q->OSEventType);
//	CuAssertIntEquals(tc, Curr_expected_tps_reading_q->OSEventGrp, expected_tps_reading_q->OSEventGrp);
//	CuAssertIntEquals(tc, Curr_expected_tps_reading_q->OSEventCnt, expected_tps_reading_q->OSEventCnt);
//	CuAssertStrEquals(tc, Curr_expected_tps_reading_q->OSEventName, expected_tps_reading_q->OSEventName);
//}
//void Test_get_new_tps_reading_sem(CuTest* tc){
//	OS_EVENT* Curr_get_new_tps_reading_sem = get_new_tps_reading_sem();
//	CuAssertTrue(tc,Curr_get_new_tps_reading_sem != NULL);
//	CuAssertIntEquals(tc, Curr_get_new_tps_reading_sem->OSEventType, new_tps_reading_available->OSEventType);
//	CuAssertIntEquals(tc, Curr_get_new_tps_reading_sem->OSEventGrp, new_tps_reading_available->OSEventGrp);
//	CuAssertIntEquals(tc, Curr_get_new_tps_reading_sem->OSEventCnt, new_tps_reading_available->OSEventCnt);
//	CuAssertStrEquals(tc, Curr_get_new_tps_reading_sem->OSEventName, new_tps_reading_available->OSEventName);
//}
//void Test_get_motor_cmd_q(CuTest* tc){
//	OS_EVENT* motor_cmd_q = get_motor_cmd_q();
//	CuAssertTrue(tc,motor_cmd_q != NULL);
//	CuAssertIntEquals(tc, motor_cmd_q->OSEventType, motor_cmd_q->OSEventType);
//	CuAssertIntEquals(tc, motor_cmd_q->OSEventGrp, motor_cmd_q->OSEventGrp);
//	CuAssertIntEquals(tc, motor_cmd_q->OSEventCnt, motor_cmd_q->OSEventCnt);
//	CuAssertStrEquals(tc, motor_cmd_q->OSEventName, motor_cmd_q->OSEventName);
//}
//void Test_get_expected_tps_reading(CuTest* tc);
//void Test_set_new_motor_position(CuTest* tc);
//
//
///*-------------------------------------------------------------------------*
// * failure_handler_proc Test
// *-------------------------------------------------------------------------*/
//void Test_get_failure_msg_q(CuTest* tc){
//	OS_EVENT* Curr_get_failure_msg_q = get_failure_msg_q();
//	CuAssertTrue(tc,Curr_get_failure_msg_q != NULL);
//	CuAssertIntEquals(tc, Curr_get_failure_msg_q->OSEventType, failure_code_q->OSEventType);
//	CuAssertIntEquals(tc, Curr_get_failure_msg_q->OSEventGrp, failure_code_q->OSEventGrp);
//	CuAssertIntEquals(tc, Curr_get_failure_msg_q->OSEventCnt, failure_code_q->OSEventCnt);
//	CuAssertStrEquals(tc, Curr_get_failure_msg_q->OSEventName, failure_code_q->OSEventName);
//}

///*-------------------------------------------------------------------------*
// * util Test
// *-------------------------------------------------------------------------*/
void Test_int16U_changed_by_threshold(CuTest* tc){
	BOOL larger = int32U_changed_by_threshold(CHANGED_BY_THRESHOLD_INPUT_2,CHANGED_BY_THRESHOLD_INPUT_1,APPS_VALUE_CHANGE_THRESHOLD);
	CuAssert(tc,"changed", larger == TRUE);
	BOOL smaller = int32U_changed_by_threshold(CHANGED_BY_THRESHOLD_INPUT_1,CHANGED_BY_THRESHOLD_INPUT_2,APPS_VALUE_CHANGE_THRESHOLD);
	CuAssert(tc,"changed", smaller == TRUE);
	BOOL same = int32U_changed_by_threshold(CHANGED_BY_THRESHOLD_INPUT_3,CHANGED_BY_THRESHOLD_INPUT_1,APPS_VALUE_CHANGE_THRESHOLD);
	CuAssert(tc,"not changed", same == FALSE);
}

void Test_int16U_differ_by_percent(CuTest* tc){
	BOOL bothZero = int32U_differ_by_percent(DIFFER_BY_PERCENT_INPUT_1,DIFFER_BY_PERCENT_INPUT_1,TPS_VALUE_DIFFERENCE_PERCENT,PERCENT_DIFF_ACCURACY);
	CuAssert(tc,"agree", bothZero == FALSE);
	BOOL smallerZero = int32U_differ_by_percent(DIFFER_BY_PERCENT_INPUT_1,DIFFER_BY_PERCENT_INPUT_4,TPS_VALUE_DIFFERENCE_PERCENT,PERCENT_DIFF_ACCURACY);
	CuAssert(tc,"agree", smallerZero == TRUE);
	BOOL twiceLarger = int32U_differ_by_percent(DIFFER_BY_PERCENT_INPUT_3,DIFFER_BY_PERCENT_INPUT_4,TPS_VALUE_DIFFERENCE_PERCENT,PERCENT_DIFF_ACCURACY);
	CuAssert(tc,"not agree", twiceLarger == TRUE);
}

/*-------------------------------------------------------------------------*
 * main
 *-------------------------------------------------------------------------*/

CuSuite* CuGetSuite(void){
	CuSuite* suite = CuSuiteNew();

//	SUITE_ADD_TEST(suite, Test_get_expected_motor_pos_q);
//	SUITE_ADD_TEST(suite, Test_get_new_tps_reading_sem);
//	SUITE_ADD_TEST(suite, Test_get_motor_cmd_q);
//	SUITE_ADD_TEST(suite, Test_get_expected_tps_reading);
//	SUITE_ADD_TEST(suite, Test_set_new_motor_position);
//	SUITE_ADD_TEST(suite, Test_get_failure_msg_q);
	SUITE_ADD_TEST(suite, Test_int16U_changed_by_threshold);
	SUITE_ADD_TEST(suite, Test_int16U_differ_by_percent);

	return suite;
}

int run_all_unit_tests(void){
	CuSuite* suite = CuSuiteNew();
	CuString* output;
	CuSuiteAddSuite(suite, CuGetSuite());
	CuStringInit(output);
	CuSuiteRun(suite);
	CuSuiteSummary(suite, output);
	CuSuiteDetails(suite, output);
	printf("%s\n", output->buffer);
	return suite->failCount;
}

void Test_pwm_gen(INT32U period, INT16U duty){
	pwm_gen_module* ptr = get_new_pwm_module(PWM_GENERATOR_TEST_AVALON_SLAVE_PERIOD_BASE, PWM_GENERATOR_TEST_AVALON_SLAVE_DUTY_BASE, PWM_GENERATOR_TEST_AVALON_SLAVE_CONTROL_BASE, period, duty);
	enable_pwm_output(ptr);
	free(ptr);
}

void Test_throttle_open(INT16U percent){
	pwm_gen_module* pwm_throttle_close = get_new_pwm_module(PWM_GENERATOR_THROTTLE_CLOSE_AVALON_SLAVE_PERIOD_BASE,
			PWM_GENERATOR_THROTTLE_CLOSE_AVALON_SLAVE_DUTY_BASE,
			PWM_GENERATOR_THROTTLE_CLOSE_AVALON_SLAVE_CONTROL_BASE,
			MOTOR_PWM_PERIOD_TICKS,
			PWM_DUTY_CYCLE_HIGH);
	pwm_gen_module* pwm_throttle_open = get_new_pwm_module(PWM_GENERATOR_THROTTLE_OPEN_AVALON_SLAVE_PERIOD_BASE,
			PWM_GENERATOR_THROTTLE_OPEN_AVALON_SLAVE_DUTY_BASE,
			PWM_GENERATOR_THROTTLE_OPEN_AVALON_SLAVE_CONTROL_BASE,
			MOTOR_PWM_PERIOD_TICKS,
			percent);
	enable_pwm_output(pwm_throttle_open);
	enable_pwm_output(pwm_throttle_close);
	free(pwm_throttle_close);
	free(pwm_throttle_open);
}

void Test_throttle_close(INT16U percent){
	pwm_gen_module* pwm_throttle_close = get_new_pwm_module(PWM_GENERATOR_THROTTLE_CLOSE_AVALON_SLAVE_PERIOD_BASE,
			PWM_GENERATOR_THROTTLE_CLOSE_AVALON_SLAVE_DUTY_BASE,
			PWM_GENERATOR_THROTTLE_CLOSE_AVALON_SLAVE_CONTROL_BASE,
			MOTOR_PWM_PERIOD_TICKS,
			percent);
	pwm_gen_module* pwm_throttle_open = get_new_pwm_module(PWM_GENERATOR_THROTTLE_OPEN_AVALON_SLAVE_PERIOD_BASE,
			PWM_GENERATOR_THROTTLE_OPEN_AVALON_SLAVE_DUTY_BASE,
			PWM_GENERATOR_THROTTLE_OPEN_AVALON_SLAVE_CONTROL_BASE,
			MOTOR_PWM_PERIOD_TICKS,
			PWM_DUTY_CYCLE_HIGH);
	enable_pwm_output(pwm_throttle_open);
	enable_pwm_output(pwm_throttle_close);
	free(pwm_throttle_close);
	free(pwm_throttle_open);
}
