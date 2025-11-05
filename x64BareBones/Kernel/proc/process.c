#include "process.h"
#include "../sched/include/scheduler.h"
#include "pmem.h"

void startWrapper(const mainFuncPtr main, const int argc, char *argv[])
{
	int returnCode = main(argc, argv);
	// TODO: Hacer el exit()
	// exit(returnCode);
}

void createProcess(const mainFuncPtr main, const int argc, char *argv[])
{
	// TODO: Validar malloc
	void *stackStart = mem_alloc(STACK_SIZE);
	void *stackEnd = stackStart + STACK_SIZE;

	void *processStackPointer = initializeProcess(startWrapper, stackEnd, main, argc, argv);
	addProcess(processStackPointer);
}

void deleteProcess(uint64_t pid){
	terminateProcess(pid);
}