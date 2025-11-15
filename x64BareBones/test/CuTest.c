// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <assert.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "CuTest.h"

#define STRING_MAX 256

/*-------------------------------------------------------------------------*
 * CuStr
 *-------------------------------------------------------------------------*/

char* CuStrAlloc(int size) {
	char* newStr = (char*) malloc( size );
	return newStr;
}

char* CuStrCopy(const char* old) {
	int len = strlen(old);
	char* newStr = CuStrAlloc(len + 1);
	strcpy(newStr, old);
	return newStr;
}

/*-------------------------------------------------------------------------*
 * CuString
 *-------------------------------------------------------------------------*/

CuString* CuStringNew(void) {
	CuString* str = (CuString*) malloc(sizeof(CuString));
	str->buffer = (char*)malloc(sizeof(char) * 256);
	str->buffer[0] = '\0';
	str->length = 0;
	str->size = 256;
	return str;
}

void CuStringDelete(CuString *str) {
	if (!str) return;
	free(str->buffer);
	free(str);
}

void CuStringResize(CuString* str, int newSize) {
	char* newBuffer = (char*)malloc(sizeof(char) * newSize);
	int i;
	for (i = 0 ; i < str->length ; ++i) newBuffer[i] = str->buffer[i];
	newBuffer[str->length] = '\0';
	free(str->buffer);
	str->buffer = newBuffer;
	str->size = newSize;
}

void CuStringAppend(CuString* str, const char* text) {
	int newLength = str->length + strlen(text);
	if (newLength >= str->size) CuStringResize(str, str->size * 2);
	strcat(str->buffer, text);
	str->length = newLength;
}

void CuStringAppendChar(CuString* str, char ch) {
	int newLength = str->length + 1;
	if (newLength >= str->size) CuStringResize(str, str->size * 2);
	str->buffer[str->length] = ch;
	str->buffer[newLength] = '\0';
	str->length = newLength;
}

void CuStringAppendFormat(CuString* str, const char* format, ...) {
	char buf[STRING_MAX];
	va_list argp;
	va_start(argp, format);
	vsprintf(buf, format, argp);
	va_end(argp);
	CuStringAppend(str, buf);
}

void CuStringInsertChar(CuString* str, int pos, char ch) {
	int newLength = str->length + 1;
	if (newLength >= str->size) CuStringResize(str, str->size * 2);
	int i;
	for (i = str->length ; i > pos ; --i) str->buffer[i] = str->buffer[i - 1];
	str->buffer[pos] = ch;
	str->buffer[newLength] = '\0';
	str->length = newLength;
}

int CuStringLength(CuString* str) {
	return str->length;
}

void CuStringClear(CuString* str) {
	str->length = 0;
	str->buffer[0] = '\0';
}

/*-------------------------------------------------------------------------*
 * CuTest
 *-------------------------------------------------------------------------*/

CuTest* CuTestNew(const char* name, TestFunction function) {
	CuTest* tc = (CuTest*) malloc(sizeof(CuTest));
	tc->name = CuStrCopy(name);
	tc->function = function;
	tc->failed = 0;
	tc->ran = 0;
	tc->message = NULL;
	tc->jumpBuf = NULL;
	return tc;
}

void CuTestDelete(CuTest *t) {
	if (!t) return;
	free(t->name);
	free(t);
}

void CuTestRun(CuTest* tc) {
	jmp_buf buf;
	tc->jumpBuf = &buf;
	if (setjmp(buf) == 0) {
		tc->ran = 1;
		(tc->function)(tc);
	}
	tc->jumpBuf = NULL;
}

static void CuFailInternal(CuTest* tc, const char* file, int line, CuString* string) {
	char buf[STRING_MAX];
	sprintf(buf, "%s:%d: ", file, line);
	int i;
	for (i = strlen(buf) - 1 ; i >= 0 ; --i) CuStringInsertChar(string, 0, buf[i]);
	tc->failed = 1;
	tc->message = CuStrCopy(string->buffer);
	if (tc->jumpBuf != 0) longjmp(*(tc->jumpBuf), 1);
}

void CuAssert(CuTest* tc, const char* message, int condition) {
	if (!condition) {
		CuString string;
		string.buffer = (char*)malloc(STRING_MAX);
		string.length = 0;
		string.size = STRING_MAX;
		CuStringAppend(&string, (char*)message);
		CuFailInternal(tc, "CuTest.c", 0, &string);
	}
}

void CuAssertTrue(CuTest* tc, int condition) {
	if (!condition) {
		CuAssert(tc, "Expected true but was false", 0);
	}
}

void CuAssertFalse(CuTest* tc, int condition) {
	if (condition) {
		CuAssert(tc, "Expected false but was true", 0);
	}
}

void CuAssertPtrEquals(CuTest* tc, const void* expected, const void* actual) {
	if (expected != actual) {
		CuString string;
		string.buffer = (char*)malloc(STRING_MAX);
		string.length = 0;
		string.size = STRING_MAX;
		sprintf(string.buffer, "Expected pointer <%p> but was <%p>", expected, actual);
		CuFailInternal(tc, "CuTest.c", 0, &string);
	}
}

void CuAssertPtrNotNull(CuTest* tc, const void* pointer) {
	if (pointer == NULL) {
		CuAssert(tc, "Expected pointer to be not null", 0);
	}
}

void CuAssertPtrNull(CuTest* tc, const void* pointer) {
	if (pointer != NULL) {
		CuString string;
		string.buffer = (char*)malloc(STRING_MAX);
		string.length = 0;
		string.size = STRING_MAX;
		sprintf(string.buffer, "Expected pointer to be null but was <%p>", pointer);
		CuFailInternal(tc, "CuTest.c", 0, &string);
	}
}

void CuAssertIntEquals(CuTest* tc, int expected, int actual) {
	if (expected != actual) {
		CuString string;
		string.buffer = (char*)malloc(STRING_MAX);
		string.length = 0;
		string.size = STRING_MAX;
		sprintf(string.buffer, "Expected <%d> but was <%d>", expected, actual);
		CuFailInternal(tc, "CuTest.c", 0, &string);
	}
}

void CuAssertIntNotEquals(CuTest* tc, int expected, int actual) {
	if (expected == actual) {
		CuString string;
		string.buffer = (char*)malloc(STRING_MAX);
		string.length = 0;
		string.size = STRING_MAX;
		sprintf(string.buffer, "Expected different ints but were both <%d>", expected);
		CuFailInternal(tc, "CuTest.c", 0, &string);
	}
}

void CuAssertDblEquals(CuTest* tc, double expected, double actual, double delta) {
	double diff = expected - actual;
	if (diff < 0) diff = -diff;
	if (diff > delta) {
		CuString string;
		string.buffer = (char*)malloc(STRING_MAX);
		string.length = 0;
		string.size = STRING_MAX;
		sprintf(string.buffer, "Expected <%lf> but was <%lf>", expected, actual);
		CuFailInternal(tc, "CuTest.c", 0, &string);
	}
}

void CuAssertDblNotEquals(CuTest* tc, double expected, double actual, double delta) {
	double diff = expected - actual;
	if (diff < 0) diff = -diff;
	if (diff <= delta) {
		CuString string;
		string.buffer = (char*)malloc(STRING_MAX);
		string.length = 0;
		string.size = STRING_MAX;
		sprintf(string.buffer, "Expected different doubles but were both <%lf>", expected);
		CuFailInternal(tc, "CuTest.c", 0, &string);
	}
}

void CuAssertStrEquals(CuTest* tc, const char* expected, const char* actual) {
	if (strcmp(expected, actual) != 0) {
		CuString string;
		string.buffer = (char*)malloc(STRING_MAX);
		string.length = 0;
		string.size = STRING_MAX;
		sprintf(string.buffer, "Expected <%s> but was <%s>", expected, actual);
		CuFailInternal(tc, "CuTest.c", 0, &string);
	}
}

void CuAssertStrNotEquals(CuTest* tc, const char* expected, const char* actual) {
	if (strcmp(expected, actual) == 0) {
		CuString string;
		string.buffer = (char*)malloc(STRING_MAX);
		string.length = 0;
		string.size = STRING_MAX;
		sprintf(string.buffer, "Expected different strings but were both <%s>", expected);
		CuFailInternal(tc, "CuTest.c", 0, &string);
	}
}

void CuAssertStrNEquals(CuTest* tc, const char* expected, const char* actual, int n) {
	if (strncmp(expected, actual, n) != 0) {
		CuString string;
		string.buffer = (char*)malloc(STRING_MAX);
		string.length = 0;
		string.size = STRING_MAX;
		sprintf(string.buffer, "Expected <%.*s> but was <%.*s>", n, expected, n, actual);
		CuFailInternal(tc, "CuTest.c", 0, &string);
	}
}

void CuAssertStrNNotEquals(CuTest* tc, const char* expected, const char* actual, int n) {
	if (strncmp(expected, actual, n) == 0) {
		CuString string;
		string.buffer = (char*)malloc(STRING_MAX);
		string.length = 0;
		string.size = STRING_MAX;
		sprintf(string.buffer, "Expected different strings but were both <%.*s>", n, expected);
		CuFailInternal(tc, "CuTest.c", 0, &string);
	}
}

/*-------------------------------------------------------------------------*
 * CuSuite
 *-------------------------------------------------------------------------*/

CuSuite* CuSuiteNew(void) {
	CuSuite* testSuite = (CuSuite*) malloc(sizeof(CuSuite));
	testSuite->count = 0;
	testSuite->failCount = 0;
	testSuite->tests = (CuTest**) malloc(sizeof(CuTest*) * 1024);
	return testSuite;
}

void CuSuiteDelete(CuSuite *testSuite) {
	if (testSuite == NULL) return;
	int i;
	for (i = 0 ; i < testSuite->count ; ++i)
		CuTestDelete(testSuite->tests[i]);
	free(testSuite->tests);
	free(testSuite);
}

void CuSuiteAdd(CuSuite* testSuite, CuTest *testCase) {
	testSuite->tests[testSuite->count] = testCase;
	testSuite->count++;
}

void CuSuiteAddSuite(CuSuite* testSuite, CuSuite* testSuite2) {
	int i;
	for (i = 0 ; i < testSuite2->count ; ++i)
		CuSuiteAdd(testSuite, testSuite2->tests[i]);
}

void CuSuiteRun(CuSuite* testSuite) {
	int i;
	for (i = 0 ; i < testSuite->count ; ++i) {
		CuTest* testCase = testSuite->tests[i];
		CuTestRun(testCase);
		if (testCase->failed) { testSuite->failCount += 1; }
	}
}

void CuSuiteSummary(CuSuite* testSuite, CuString* summary) {
	int i;
	for (i = 0 ; i < testSuite->count ; ++i) {
		CuTest* testCase = testSuite->tests[i];
		CuStringAppend(summary, testCase->failed ? "F" : ".");
	}
	CuStringAppend(summary, "\n\n");
}

void CuSuiteDetails(CuSuite* testSuite, CuString* details) {
	int i;
	int failCount = 0;
	if (testSuite->failCount == 0) {
		int passCount = testSuite->count - testSuite->failCount;
		const char* testWord = passCount == 1 ? "test" : "tests";
		CuStringAppendFormat(details, "OK: %d %s\n", passCount, testWord);
		return;
	}
	if (testSuite->failCount == 1)
		CuStringAppend(details, "FAIL: There was 1 failure:\n");
	else
		CuStringAppendFormat(details, "FAIL: There were %d failures:\n", testSuite->failCount);
	for (i = 0 ; i < testSuite->count ; ++i) {
		CuTest* testCase = testSuite->tests[i];
		if (testCase->failed) {
			failCount++;
			CuStringAppendFormat(details, "\n%d) %s\n", failCount, testCase->name);
			CuStringAppendFormat(details, "%s\n", testCase->message);
		}
	}
	CuStringAppend(details, "\n!!!FAILURES!!!\n");
}
