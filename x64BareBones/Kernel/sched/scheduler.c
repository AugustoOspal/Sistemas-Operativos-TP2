#include "scheduler.h"
#include "../lib/string/strings.h"

typedef enum ProcessState
{
	READY,
	RUNNING,
	BLOCKED,
	TERMINATED
} ProcessState;

typedef struct ProcessCDT
{
	void *basePointer;
	void *stack;
	uint64_t pid;
	char *nombre;
	ProcessState state;
	uint8_t priority;
	struct Process *parent;
	uint8_t quantumLeft;
	bool foreground;
} ProcessCDT;

typedef struct schedulerT
{
	ProcessADT currentProcess;
	doubleLinkedListADT processTable;
	QueueADT priorityQueues[PRIO];
	uint8_t weights[PRIO];
	uint8_t credits[PRIO];
	QueueADT blockedQueue;
} schedulerT;

schedulerT globalScheduler;
ProcessADT idleProcess;		 // Esta también puede ir adentro del schedulerT
static uint64_t lastPid = 0; // Este PID es el proximo a crear

static void refill_credits(void)
{
	for (int i = 0; i < PRIO; i++)
		globalScheduler.credits[i] = globalScheduler.weights[i];
}

void initializeScheduler()
{
	globalScheduler.processTable = newDoubleLinkedListCDT();
	for (int i = 0; i < PRIO; i++)
	{
		globalScheduler.priorityQueues[i] = NewQueue();
	}
	globalScheduler.currentProcess = NULL;

	for (int i = 0; i < PRIO; i++)
	{
		globalScheduler.weights[i] = PRIO - i;
	}
	refill_credits();

	globalScheduler.blockedQueue = NewQueue();

	createProcess("idle", idleMain, 0, NULL);
	idleProcess = Dequeue(globalScheduler.priorityQueues[DEFAULT_PRIORITY]);
}

uint64_t addProcess(void *stackPointer)
{
	// No estaria chequeando que existe scheduler

	ProcessADT newProcess = mem_alloc(sizeof(ProcessCDT));
	newProcess->pid = lastPid++;
	newProcess->stack = stackPointer;
	newProcess->state = READY;
	newProcess->quantumLeft = QUANTUM;
	newProcess->priority = DEFAULT_PRIORITY;

	addToDoubleLinkedList(globalScheduler.processTable, newProcess);
	Enqueue(globalScheduler.priorityQueues[DEFAULT_PRIORITY], newProcess);
	return newProcess->pid;
}

static bool matchPid(void *elem, void *data)
{
	ProcessADT proc = (ProcessADT) elem;
	uint64_t *targetPid = (uint64_t *) data;
	return proc->pid == *targetPid;
}

void addProcessInfo(uint64_t pid, const char *name, void *basePointer)
{
	for (int i = 0; i < PRIO; i++)
	{
		ProcessADT proc = FindInQueue(globalScheduler.priorityQueues[i], matchPid, &pid);

		if (proc)
		{
			proc->nombre = name;
			proc->basePointer = basePointer;
			proc->foreground = DEFAULT_FOREGROUND;
			return;
		}
	}
}

static int pickNextQueue(void)
{
	int sum = 0;
	for (int i = 0; i < PRIO; i++)
		if (!IsQueueEmpty(globalScheduler.priorityQueues[i]))
			sum += globalScheduler.credits[i];

	if (sum == 0)
		refill_credits();

	for (int p = 0; p < PRIO; p++)
	{
		if (globalScheduler.credits[p] > 0 && !IsQueueEmpty(globalScheduler.priorityQueues[p]))
		{
			globalScheduler.credits[p]--;
			return p;
		}
	}
	return -1;
}

static void *pickNextProcess()
{
	int queue = pickNextQueue();
	if (queue < 0)
	{
		globalScheduler.currentProcess = idleProcess;
		idleProcess->state = RUNNING;
		return idleProcess->stack;
	}
	globalScheduler.currentProcess = (ProcessADT) Dequeue(globalScheduler.priorityQueues[queue]);
	globalScheduler.currentProcess->state = RUNNING;
	globalScheduler.currentProcess->quantumLeft = QUANTUM;
	return globalScheduler.currentProcess->stack;
}

void *schedule(void *stackPointer)
{
	if (globalScheduler.currentProcess)
	{
		globalScheduler.currentProcess->stack = stackPointer;

		if (globalScheduler.currentProcess != idleProcess)
		{
			if (globalScheduler.currentProcess->state == BLOCKED)
			{
				return pickNextProcess();
			}
			if (--globalScheduler.currentProcess->quantumLeft)
			{
				return stackPointer;
			}
			globalScheduler.currentProcess->state = READY;
			uint8_t pr = globalScheduler.currentProcess->priority;
			Enqueue(globalScheduler.priorityQueues[pr], globalScheduler.currentProcess);
		}
	}

	return pickNextProcess();
}

static ProcessADT getProcessByPid(uint64_t pid)
{
	for (int i = 0; i < PRIO; i++)
	{
		ProcessADT proc = FindInQueue(globalScheduler.priorityQueues[i], matchPid, &pid);

		if (proc)
		{
			return proc;
		}
	}
	return NULL;
}

void changeProcessPriority(uint64_t pid, uint8_t newPriority)
{
	ProcessADT proc = getProcessByPid(pid);
	if (newPriority >= PRIO)
		return;
	if (!proc)
		return;
	if (proc->priority == newPriority)
		return;
	RemoveFromQueue(globalScheduler.priorityQueues[proc->priority], proc);
	proc->priority = newPriority;
	Enqueue(globalScheduler.priorityQueues[newPriority], proc);
}

void removeProcess(uint64_t pid)
{
	ProcessADT p = getProcessByPid(pid);
	if (!p)
		return;

	// sacar de la tabla global
	removeFromDoubleLinkedList(globalScheduler.processTable, p);

	// sacar de cualquier cola en la que este (si sigue en alguna)
	RemoveFromQueue(globalScheduler.priorityQueues[p->priority], p);

	// liberar memoria del stack y del PCB
	mem_free(p->stack);
	mem_free(p);
}

static inline void forceTimerInterrupt(void)
{ // TODO: hacer prolijo en asm
	__asm__ __volatile__("int $0x20");
}

void blockProcess(uint64_t pid)
{
	if (globalScheduler.currentProcess && globalScheduler.currentProcess->pid == pid)
	{
		ProcessADT running = globalScheduler.currentProcess;
		if (running->state == BLOCKED)
			return;
		running->state = BLOCKED;
		Enqueue(globalScheduler.blockedQueue, running);
		forceTimerInterrupt();
		return;
	}

	ProcessADT p = getProcessByPid(pid);
	if (!p)
		return;

	RemoveFromQueue(globalScheduler.priorityQueues[p->priority], p);
	p->state = BLOCKED;

	Enqueue(globalScheduler.blockedQueue, p);
}

void unblockProcess(uint64_t pid)
{
	ProcessADT p = FindInQueue(globalScheduler.blockedQueue, matchPid, &pid);
	if (!p)
		return;

	RemoveFromQueue(globalScheduler.blockedQueue, p);
	p->state = READY;

	Enqueue(globalScheduler.priorityQueues[p->priority], p);
}

void yield()
{
	globalScheduler.currentProcess->quantumLeft = 1;
	forceTimerInterrupt();
}

uint64_t getPid()
{
	return globalScheduler.currentProcess->pid;
}

char *getProcessInfo(uint64_t pid)
{
	static char buffer[256];
	ProcessADT p = getProcessByPid(pid);

	if (!p)
	{
		buffer[0] = '\0';
		return buffer;
	}

	const char *state_str;
	switch (p->state)
	{
		case READY:
			state_str = "READY";
			break;
		case RUNNING:
			state_str = "RUNNING";
			break;
		case BLOCKED:
			state_str = "BLOCKED";
			break;
		case TERMINATED:
			state_str = "TERMINATED";
			break;
		default:
			state_str = "UNKNOWN";
			break;
	}

	ksprintf(buffer, "PID: %lu | Name: %s | Priority: %u | State: %s | Foreground: %s | Stack: 0x%lx | BP: 0x%lx",
			 p->pid, p->nombre ? p->nombre : "(unnamed)", p->priority, state_str, p->foreground ? "Yes" : "No",
			 (unsigned long) p->stack, (unsigned long) p->basePointer);

	return buffer;
}

