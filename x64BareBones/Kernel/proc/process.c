#include "process.h"
#include "pmem.h"

void startWrapper(const mainFuncPtr main, const int argc, char *argv[])
{
    int returnCode = main(argc, argv);
    //exit(returnCode);
}

void createProcess(const mainFuncPtr main, const int argc, char *argv[])
{
    void *stackStart = mem_alloc(STACK_SIZE);
    void *stackEnd = stackStart + STACK_SIZE;

    void *processStackPointer = initializeProcess(startWrapper, stackEnd, main, argc, argv);
}
