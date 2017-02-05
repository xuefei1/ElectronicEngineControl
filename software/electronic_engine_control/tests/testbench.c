/*
 * testbench.c
 *
 *  Created on: Feb 3, 2017
 *      Author: Fred,
 *
 *      Runs all the unit tests
 *
 *	Reference: https://github.com/ennorehling/cutest
 */

#include "../test_config.h"
#include <assert.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "CuTest.h"

/*-------------------------------------------------------------------------*
 * Helper functions
 *-------------------------------------------------------------------------*/

#define CompareAsserts(tc, message, expected, actual)  X_CompareAsserts((tc), __FILE__, __LINE__, (message), (expected), (actual))

static void X_CompareAsserts(CuTest* tc, const char *file, int line, const char* message, const char* expected, CuString *actual)
{
	int mismatch;
	if (expected == NULL || actual == NULL || actual==NULL) {
		mismatch = (expected != NULL || actual != NULL);
	} else {
		const char *front = __FILE__ ":";
		const size_t frontLen = strlen(front);
		const size_t expectedLen = strlen(expected);

		const char *matchStr = actual->buffer;

		mismatch = (strncmp(matchStr, front, frontLen) != 0);
		if (!mismatch) {
			matchStr = strchr(matchStr + frontLen, ':');
			mismatch |= (matchStr == NULL || strncmp(matchStr, ": ", 2));
			if (!mismatch) {
				matchStr += 2;
				mismatch |= (strncmp(matchStr, expected, expectedLen) != 0);
			}
		}
	}

	CuAssert_Line(tc, file, line, message, !mismatch);
}

/*-------------------------------------------------------------------------*
 * apps_motor_proc Test
 *-------------------------------------------------------------------------*/
void Test_apps_motor_task(CuTest* tc)；
void Test_get_expected_motor_pos_q(CuTest* tc);
void Test_get_new_tps_reading_sem(CuTest* tc);
void Test_get_motor_cmd_q(CuTest* tc);
void Test_get_expected_tps_reading(CuTest* tc);
void Test_set_new_motor_position(CuTest* tc);


/*-------------------------------------------------------------------------*
 * failure_handler_proc Test
 *-------------------------------------------------------------------------*/
void Test_failure_handler_proc(CuTest* tc);
void Test_get_failure_msg_q(CuTest* tc);

/*-------------------------------------------------------------------------*
 * tps_task Test
 *-------------------------------------------------------------------------*/
void Test_tps_task(CuTest* tc);

/*-------------------------------------------------------------------------*
 * ui_proc Test
 *-------------------------------------------------------------------------*/
void Test_ui_proc(CuTest* tc);
void Test_get_lcd_msg_q(CuTest* tc);

/*-------------------------------------------------------------------------*
 * util Test
 *-------------------------------------------------------------------------*/
void Test_int16U_changed_by_threshold(CuTest* tc);
void Test_int16U_differ_by_percent(CuTest* tc){
	INT16U num0 = 0;
	INT16U num1 = 1;
	INT16U num2 = 10;
	INT16U num3 = 100;
	INT16U perc0 = 0;
	INT16U perc1 = 25;
	INT16U perc2 = 50;
	INT16U accu; //what is this?

	BOOL boothZero = int16U_differ_by_percent(num0,num0,perc0,accu);
	CuAssertTrue(tc,boothZero);
	BOOL smallerZero = int16U_differ_by_percent(num0,num3,perc1,accu);
	CuAssertTrue(tc,!smallerZero);

}

/*-------------------------------------------------------------------------*
 * main
 *-------------------------------------------------------------------------*/

CuSuite* CuGetSuite(void)
{
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, Test_apps_motor_task);
	SUITE_ADD_TEST(suite, Test_get_expected_motor_pos_q);
	SUITE_ADD_TEST(suite, Test_get_new_tps_reading_sem);
	SUITE_ADD_TEST(suite, Test_get_motor_cmd_q);
	SUITE_ADD_TEST(suite, Test_get_expected_tps_reading);
	SUITE_ADD_TEST(suite, Test_set_new_motor_position);
	SUITE_ADD_TEST(suite, Test_failure_handler_proc);
	SUITE_ADD_TEST(suite, Test_get_failure_msg_q);
	SUITE_ADD_TEST(suite, Test_tps_task);
	SUITE_ADD_TEST(suite, Test_ui_proc);
	SUITE_ADD_TEST(suite, Test_get_lcd_msg_q);
	SUITE_ADD_TEST(suite, Test_int16U_changed_by_threshold);
	SUITE_ADD_TEST(suite, Test_int16U_differ_by_percent);

	return suite;
}