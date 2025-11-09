#include "include/semaphore.h"



static doubleLinkedListADT semaphoreList = NULL;

static bool matchSemName(void *elem, void *name);
static bool matchPid(void *elem, void *pid);
static int cmpSem(void *a, void *b);
static semaphoreP createSem();

void semaphoresInit(void)
{
	semaphoreList = newDoubleLinkedListADT();
}

semaphoreP semOpen(const char *name, const int value)
{
	if (!semaphoreList || !name)
		return NULL;

	semaphoreP sem = getSem(name);
	if (sem)
	{
		if (!sem->pendingDestruction)
			return sem;
		return NULL;
	}

	sem = createSem();
	if (!sem)
		return NULL;

	sem->value = value;
	sem->name = strDup(name);
	sem->lock = 0;
	sem->pendingDestruction = false;
	addToDoubleLinkedListSorted(semaphoreList, sem, cmpSem);

	addToDoubleLinkedList(sem->linkedProcesses, (void *) getPid());
	return sem;
}

void semClose(const semaphoreP sem)
{
	removeFromDoubleLinkedList(sem->linkedProcesses, matchPid, (void *) getPid());
}

int sem_unlink(const char *name)
{
	const semaphoreP sem = getSem(name);
	if (!sem)
		return -1;

	removeFromDoubleLinkedList(semaphoreList, matchSemName, (void *) name);
	sem->pendingDestruction = true;
	return 0;
}

void semPost(const semaphoreP sem)
{
	acquire(&sem->lock);
	if (++sem->value <= 0)
	{
		unblockProcess((uint64_t) Dequeue(sem->waitingProcesses));
	}
	release(&sem->lock);
	if (isListEmpty(sem->linkedProcesses) && IsQueueEmpty(sem->waitingProcesses) && sem->pendingDestruction)
	{
		FreeQueue(sem->waitingProcesses);
		FreeDoubleLinkedListCDT(sem->linkedProcesses);
		mem_free(sem);
	}
}

void semWait(const semaphoreP sem)
{
	// Afuera del lock para que pase menos tiempo loqueado
	const uint64_t currentPID = getPid();
	acquire(&sem->lock);
	if (--sem->value < 0)
	{
		Enqueue(sem->waitingProcesses, (void *) currentPID);
		addProcessToBlockQueue(currentPID);
		release(&sem->lock);
		yield();
		return;
	}
	release(&sem->lock);
}

int semTryWait(const semaphoreP sem)
{
	if (try_acquire(&sem->lock))
		return -1;

	if (--sem->value < 0)
	{
		sem->value++;
		release(&sem->lock);
		return -1;
	}

	release(&sem->lock);
	return 0;
}

int semGetValue(const semaphoreP sem, int *sval)
{
	if (!sem || !sval)
		return -1;

	acquire(&sem->lock);

	if (IsQueueEmpty(sem->waitingProcesses))
	{
		*sval = sem->value;
	}
	else
	{
		*sval = -1 * QueueLength(sem->waitingProcesses);
	}

	release(&sem->lock);

	return 0;
}

static bool matchSemName(void *elem, void *name)
{
	const semaphoreP sem = elem;
	const char *searchName = (char *) name;

	int i = 0;
	while (sem->name[i] != '\0' && searchName[i] != '\0')
	{
		if (sem->name[i] != searchName[i])
			return false;
		i++;
	}

	return sem->name[i] == searchName[i];
}

static bool matchPid(void *elem, void *pid)
{
	const uint64_t elemPid = (uint64_t) elem;
	const uint64_t searchPid = (uint64_t) pid;
	return elemPid == searchPid;
}

static int cmpSem(void *a, void *b)
{
	const semaphoreP sem1 = a;
	const semaphoreP sem2 = b;

	int i = 0;
	while (sem1->name[i] != '\0' && sem2->name[i] != '\0')
	{
		if (sem1->name[i] < sem2->name[i])
			return -1;
		if (sem1->name[i] > sem2->name[i])
			return 1;
		i++;
	}

	if (sem1->name[i] == '\0' && sem2->name[i] != '\0')
		return -1;
	if (sem1->name[i] != '\0' && sem2->name[i] == '\0')
		return 1;

	return 0;
}

semaphoreP getSem(const char *name)
{
	if (!semaphoreList || !name)
		return NULL;

	return findInDoubleLinkedList(semaphoreList, matchSemName, (void *) name);
}

static semaphoreP createSem()
{
	const semaphoreP newSem = mem_alloc(sizeof(semaphore));
	newSem->waitingProcesses = NewQueue();
	newSem->linkedProcesses = newDoubleLinkedListADT();

	if (!newSem->linkedProcesses || !newSem->waitingProcesses)
		return NULL;

	return newSem;
}