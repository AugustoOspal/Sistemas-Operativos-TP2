#include "process.h"
#include "../sched/include/scheduler.h"
#include "pmem.h"

extern void *initializeProcess(startWrapperPtr startWrapper, void *stack, mainFuncPtr main, int argc, char *argv[]);

void startWrapper(const mainFuncPtr main, const int argc, char *argv[])
{
	int returnCode = main(argc, argv);
	// TODO: Hacer el exit()
	// exit(returnCode);
}

uint64_t createProcess(const char *name, const mainFuncPtr main, const int argc, char *argv[])
{
	// TODO: Validar malloc
	void *stackStart = mem_alloc(STACK_SIZE);
	void *stackEnd = stackStart + STACK_SIZE;

	void *processStackPointer = initializeProcess(startWrapper, stackEnd, main, argc, argv);
	const uint64_t newProc = addProcess(processStackPointer);
	addProcessInfo(newProc, name, stackStart);
	return newProc;
}

void deleteProcess(const uint64_t pid)
{
	removeProcess(pid);
}