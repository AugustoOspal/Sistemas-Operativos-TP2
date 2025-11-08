#include "include/semaphores.h"
#include "include/syscallLib.h"

semaphoreP semOpen(const char *name, const int value)
{
	return sys_semOpen(name, value);
}

void semClose(const semaphoreP sem)
{
	sys_semClose(sem);
}

void semWait(const semaphoreP sem)
{
	sys_semWait(sem);
}

void semPost(const semaphoreP sem)
{
	sys_semPost(sem);
}

int semTryWait(const semaphoreP sem)
{
	return sys_semTryWait(sem);
}

int semGetValue(const semaphoreP sem, int *sval)
{
	return sys_semGetValue(sem, sval);
}

int semUnlink(const char *name)
{
	return sys_semUnlink(name);
}
