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
 * ADC Test
 *-------------------------------------------------------------------------*/






/*-------------------------------------------------------------------------*
 * main
 *-------------------------------------------------------------------------*/

CuSuite* CuGetSuite(void)
{
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, );
	SUITE_ADD_TEST(suite, );
	SUITE_ADD_TEST(suite, );
	SUITE_ADD_TEST(suite, );
	SUITE_ADD_TEST(suite, );
	SUITE_ADD_TEST(suite, );
	SUITE_ADD_TEST(suite, );
	SUITE_ADD_TEST(suite, );

	return suite;
}