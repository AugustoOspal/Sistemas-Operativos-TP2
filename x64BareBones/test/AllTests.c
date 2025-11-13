#include <stdio.h>

#include "CuTest.h"
#include "TestMemoryManager.h"
#include "../Kernel/semaphore/TestSemaphore.h"
#include "../Kernel/ipc/TestPipe.h"

void RunAllTests(void) {
    CuString *output = CuStringNew();
    CuSuite* suite = CuSuiteNew();

    CuSuiteAddSuite(suite, getMemoryManagerTestSuite());
    CuSuiteAddSuite(suite, getSemaphoreTestSuite());
    CuSuiteAddSuite(suite, getPipeTestSuite());

    CuSuiteRun(suite);

    CuSuiteSummary(suite, output);
    CuSuiteDetails(suite, output);

    printf("%s\n", output->buffer);
}

int main(void) {
    RunAllTests();
    return 0;
}