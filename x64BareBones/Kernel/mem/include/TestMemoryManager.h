#ifndef _TEST_MEMORY_MANAGER_H_
#define _TEST_MEMORY_MANAGER_H_

#include "CuTest.h"

typedef void (*Test)(CuTest *const cuTest);

CuSuite *getMemoryManagerTestSuite(void);

#endif
