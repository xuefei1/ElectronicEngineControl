/*
 * testbench.c
 *
 *  Created on: Feb 3, 2017
 *      Author: Fred,
 *
 *      Runs all the unit tests
 *
 *	Reference: https://github.com/ennorehling/cutest
 *	
 */

#include "../test_config.h"
#include <assert.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "CuTest.h"

/*-------------------------------------------------------------------------*
 * apps_motor_proc Test
 *-------------------------------------------------------------------------*/
void Test_get_expected_motor_pos_q(CuTest* tc){
	OS_EVENT* Curr_expected_tps_reading_q = get_expected_motor_pos_q();
	CuAssertIntEquals(tc, Curr_expected_tps_reading_q->OSEventType, expected_tps_reading_q->OSEventType);
	CuAssertIntEquals(tc, Curr_expected_tps_reading_q->OSEventGrp, expected_tps_reading_q->OSEventGrp);
	CuAssertIntEquals(tc, Curr_expected_tps_reading_q->OSEventCnt, expected_tps_reading_q->OSEventCnt);
	CuAssertStrEquals(tc, Curr_expected_tps_reading_q->OSEventName, expected_tps_reading_q->OSEventName);
}
void Test_get_new_tps_reading_sem(CuTest* tc){
	OS_EVENT* Curr_get_new_tps_reading_sem = get_new_tps_reading_sem();
	CuAssertIntEquals(tc, Curr_get_new_tps_reading_sem->OSEventType, new_tps_reading_available->OSEventType);
	CuAssertIntEquals(tc, Curr_get_new_tps_reading_sem->OSEventGrp, new_tps_reading_available->OSEventGrp);
	CuAssertIntEquals(tc, Curr_get_new_tps_reading_sem->OSEventCnt, new_tps_reading_available->OSEventCnt);
	CuAssertStrEquals(tc, Curr_get_new_tps_reading_sem->OSEventName, new_tps_reading_available->OSEventName);
}
void Test_get_motor_cmd_q(CuTest* tc){
	OS_EVENT* Curr_get_motor_cmd_q = get_motor_cmd_q();
	CuAssertIntEquals(tc, Curr_get_motor_cmd_q->OSEventType, motor_cmd_q->OSEventType);
	CuAssertIntEquals(tc, Curr_get_motor_cmd_q->OSEventGrp, motor_cmd_q->OSEventGrp);
	CuAssertIntEquals(tc, Curr_get_motor_cmd_q->OSEventCnt, motor_cmd_q->OSEventCnt);
	CuAssertStrEquals(tc, Curr_get_motor_cmd_q->OSEventName, motor_cmd_q->OSEventName);
}
void Test_get_expected_tps_reading(CuTest* tc);
void Test_set_new_motor_position(CuTest* tc);


/*-------------------------------------------------------------------------*
 * failure_handler_proc Test
 *-------------------------------------------------------------------------*/
void Test_get_failure_msg_q(CuTest* tc){
	OS_EVENT* Curr_get_failure_msg_q = get_failure_msg_q();
	CuAssertIntEquals(tc, Curr_get_failure_msg_q->OSEventType, failure_code_q->OSEventType);
	CuAssertIntEquals(tc, Curr_get_failure_msg_q->OSEventGrp, failure_code_q->OSEventGrp);
	CuAssertIntEquals(tc, Curr_get_failure_msg_q->OSEventCnt, failure_code_q->OSEventCnt);
	CuAssertStrEquals(tc, Curr_get_failure_msg_q->OSEventName, failure_code_q->OSEventName);
}

/*-------------------------------------------------------------------------*
 * ui_proc Test
 *-------------------------------------------------------------------------*/
void Test_get_lcd_msg_q(CuTest* tc){
	OS_EVENT* Curr_get_lcd_msg_q = get_lcd_msg_q();
	CuAssertIntEquals(tc, Curr_get_lcd_msg_q->OSEventType, lcd_msg_q->OSEventType);
	CuAssertIntEquals(tc, Curr_get_lcd_msg_q->OSEventGrp, lcd_msg_q->OSEventGrp);
	CuAssertIntEquals(tc, Curr_get_lcd_msg_q->OSEventCnt, lcd_msg_q->OSEventCnt);
	CuAssertStrEquals(tc, Curr_get_lcd_msg_q->OSEventName, lcd_msg_q->OSEventName);
}

/*-------------------------------------------------------------------------*
 * util Test
 *-------------------------------------------------------------------------*/
void Test_int16U_changed_by_threshold(CuTest* tc){
	INT16U num0 = 100;
	INT16U num1 = 500;
	INT16U num2 = 150;

	BOOL larger = int16U_changed_by_threshold(num1,num0,APPS_VALUE_CHANGE_THRESHOLD);
	CuAssertTrue(tc,larger);
	BOOL smaller = int16U_changed_by_threshold(num0,num1,APPS_VALUE_CHANGE_THRESHOLD);
	CuAssertTrue(tc,smaller);
	BOOL same = int16U_changed_by_threshold(num2,num0,APPS_VALUE_CHANGE_THRESHOLD);
	CuAssertTrue(tc,same);
}

void Test_int16U_differ_by_percent(CuTest* tc){
	INT16U num0 = 0;
	INT16U num1 = 1;
	INT16U num2 = 10;
	INT16U num3 = 15;
	INT16U num4 = 25;

	BOOL boothZero = int16U_differ_by_percent(num0,num0,TPS_VALUE_DIFFERENCE_PERCENT,PERCENT_DIFF_ACCURACY);
	CuAssertTrue(tc,boothZero);
	BOOL smallerZero = int16U_differ_by_percent(num0,num3,TPS_VALUE_DIFFERENCE_PERCENT,PERCENT_DIFF_ACCURACY);
	CuAssertTrue(tc,!smallerZero);
	BOOL twiceLarger = int16U_differ_by_percent(num2,num3,TPS_VALUE_DIFFERENCE_PERCENT,PERCENT_DIFF_ACCURACY);
	CuAssertTrue(tc,!twiceLarger);
	
	
}

/*-------------------------------------------------------------------------*
 * main
 *-------------------------------------------------------------------------*/

CuSuite* CuGetSuite(void){
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, Test_get_expected_motor_pos_q);
	SUITE_ADD_TEST(suite, Test_get_new_tps_reading_sem);
	SUITE_ADD_TEST(suite, Test_get_motor_cmd_q);
	SUITE_ADD_TEST(suite, Test_get_expected_tps_reading);
	SUITE_ADD_TEST(suite, Test_set_new_motor_position);
	SUITE_ADD_TEST(suite, Test_get_failure_msg_q);
	SUITE_ADD_TEST(suite, Test_get_lcd_msg_q);
	SUITE_ADD_TEST(suite, Test_int16U_changed_by_threshold);
	SUITE_ADD_TEST(suite, Test_int16U_differ_by_percent);

	return suite;
}

int RunAllTests(void){
	CuSuite* suite = CuSuiteNew();

	CuSuiteAddSuite(suite, CuGetSuite());

	CuSuiteRun(suite);
	CuSuiteSummary(suite, output);
	CuSuiteDetails(suite, output);
	printf("%s\n", output->buffer);
	return suite->failCount;
}