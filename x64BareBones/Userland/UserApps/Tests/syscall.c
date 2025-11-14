#include "../../UserLib/include/syscallLib.h"
#include "./include/syscall.h"
#include <stdint.h>

int64_t my_getpid()
{
	return sys_getPid();
}

int64_t my_create_process(const char *name, mainFuncPtr main, const int argc, char *argv[],
						  int16_t fds[3], const bool foreground)
{
	return sys_createProcess(name, main, argc, argv, fds, foreground);
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

semaphoreP my_sem_open(char *sem_id, const int initialValue)
{
	return sys_semOpen(sem_id, initialValue);
}

void my_sem_wait(const semaphoreP sem)
{
	sys_semWait(sem);
}

void my_sem_post(const semaphoreP sem)
{
	sys_semPost(sem);
}

void my_sem_close(const semaphoreP sem)
{
	sys_semClose(sem);
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
