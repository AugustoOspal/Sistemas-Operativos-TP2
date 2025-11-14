#include "../../UserLib/include/syscallLib.h"
#include "./include/syscall.h"
#include <stdint.h>

int64_t my_getpid()
{
	sys_getPid();
	return 0;
}

int64_t my_create_process(const char *name, mainFuncPtr main, const int argc, char *argv[],
						  int16_t fds[3], const bool foreground)
{
	sys_createProcess(name, main, argc, argv, fds, foreground);
	return 0;
}

int64_t my_nice(const uint64_t pid, const uint64_t newPrio)
{
	sys_changeProcessPriority(pid, newPrio);
	return 0;
}

int64_t my_kill(const uint64_t pid)
{
	sys_deleteProcess(pid);
	return 0;
}

int64_t my_block(const uint64_t pid)
{
	sys_blockProcess(pid);
	return 0;
}

int64_t my_unblock(const uint64_t pid)
{
	sys_unblockProcess(pid);
	return 0;
}

int64_t my_sem_open(char *sem_id, const uint64_t initialValue)
{
	sys_semOpen(sem_id, initialValue);
	return 0;
}

int64_t my_sem_wait(char *sem_id)
{
	sys_semWait(sem_id);
	return 0;
}

int64_t my_sem_post(char *sem_id)
{
	sys_semPost(sem_id);
	return 0;
}

int64_t my_sem_close(char *sem_id)
{
	sys_semClose(sem_id);
	return 0;
}

int64_t my_yield()
{
	sys_yield();
	return 0;
}

int64_t my_wait(const int64_t pid)
{
	sys_waitPid(pid);
	return 0;
}
