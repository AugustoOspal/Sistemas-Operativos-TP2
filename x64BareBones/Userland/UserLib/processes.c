#include "include/processes.h"
#include "include/syscallLib.h"

/*
 *	@brie
 */
uint64_t createProcess(const char *name, int (*main)(int argc, char *argv[]), int argc, char *argv[])
{
	return sys_createProcess(name, main, argc, argv);
}

void deleteProcess(uint64_t pid)
{
	sys_deleteProcess(pid);
}

uint64_t getPID()
{
	return sys_getPid();
}

uint64_t getProcessesInfo(char *buffer, uint64_t bufferSize)
{
	return sys_getAllProcessesInfo(buffer, bufferSize);
}

void changeProcessPriority(uint64_t pid, uint8_t newPriority)
{
	sys_changeProcessPriority(pid, newPriority);
}

void blockProcess(uint64_t pid)
{
	sys_blockProcess(pid);
}

void unblockProcess(uint64_t pid)
{
	sys_unblockProcess(pid);
}

void yield()
{
	sys_yield();
}
