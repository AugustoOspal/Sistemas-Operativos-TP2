#ifndef CU_TEST_H
#define CU_TEST_H

#include <setjmp.h>
#include <stddef.h>

#define CU_VERSION "2.4"

/* CuString */

typedef struct {
	char* buffer;
	int length;
	int size;
} CuString;

CuString* CuStringNew(void);
void CuStringDelete(CuString *str);
void CuStringResize(CuString* str, int newSize);
void CuStringAppend(CuString* str, const char* text);
void CuStringAppendChar(CuString* str, char ch);
void CuStringAppendFormat(CuString* str, const char* format, ...);
void CuStringInsertChar(CuString* str, int pos, char ch);
int CuStringLength(CuString* str);
void CuStringClear(CuString* str);

/* CuTest */

typedef struct CuTest CuTest;

typedef void (*TestFunction)(CuTest *);

struct CuTest {
	char* name;
	TestFunction function;
	int failed;
	int ran;
	const char* message;
	jmp_buf *jumpBuf;
};

/* Creates a new CuTest */
CuTest* CuTestNew(const char* name, TestFunction function);
/* Destroys a CuTest */
void CuTestDelete(CuTest *t);
/* Runs a test and collects results */
void CuTestRun(CuTest* tc);

/* Asserts */
void CuAssert(CuTest* tc, const char* message, int condition);
void CuAssertTrue(CuTest* tc, int condition);
void CuAssertFalse(CuTest* tc, int condition);
void CuAssertPtrEquals(CuTest* tc, const void* expected, const void* actual);
void CuAssertPtrNotNull(CuTest* tc, const void* pointer);
void CuAssertPtrNull(CuTest* tc, const void* pointer);
void CuAssertIntEquals(CuTest* tc, int expected, int actual);
void CuAssertIntNotEquals(CuTest* tc, int expected, int actual);
void CuAssertDblEquals(CuTest* tc, double expected, double actual, double delta);
void CuAssertDblNotEquals(CuTest* tc, double expected, double actual, double delta);
void CuAssertStrEquals(CuTest* tc, const char* expected, const char* actual);
void CuAssertStrNotEquals(CuTest* tc, const char* expected, const char* actual);
void CuAssertStrNEquals(CuTest* tc, const char* expected, const char* actual, int n);
void CuAssertStrNNotEquals(CuTest* tc, const char* expected, const char* actual, int n);

/* CuSuite */

typedef struct {
	int count;
	CuTest** tests;
	int failCount;
} CuSuite;

/* Creates a new suite */
CuSuite* CuSuiteNew(void);
/* Destroys a suite */
void CuSuiteDelete(CuSuite *suite);
void CuSuiteAdd(CuSuite* suite, CuTest *t);
void CuSuiteAddSuite(CuSuite* suite, CuSuite* testSuite);
/* Runs all tests in a suite */
void CuSuiteRun(CuSuite* suite);
/* Prints a summary of the test results */
void CuSuiteSummary(CuSuite* suite, CuString* output);
void CuSuiteDetails(CuSuite* suite, CuString* output);

#define SUITE_ADD_TEST(suite, test) CuSuiteAdd(suite, CuTestNew(#test, test))

#endif /* CU_TEST_H */
