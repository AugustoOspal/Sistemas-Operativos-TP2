#include "scheduler.h"

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

// switchProcess();
// selectNextProcess();

typedef struct schedulerT
{
	QueueADT queue;
	ProcessADT currentProcess;
} schedulerT;

schedulerT globalScheduler;

// A lo mejor este podria ir adentro de schedulerT
ProcessADT idleProcess;

// Este PID es el proximo a crear
static uint64_t lastPid = 0;

void initializeScheduler()
{
	globalScheduler.queue = NewQueue();
	globalScheduler.currentProcess = NULL;
	createProcess(idleMain, 0, NULL);
	idleProcess = Dequeue(globalScheduler.queue);
}

ProcessADT addProcess(void *stackPointer)
{
	// No estaria chequeando que existe scheduler

	ProcessADT newProcess = mem_alloc(sizeof(ProcessCDT));
	newProcess->pid = lastPid++;
	newProcess->stack = stackPointer;
	newProcess->state = READY;
	newProcess->quantumLeft = QUANTUM;
	Enqueue(globalScheduler.queue, newProcess);
	return newProcess;
}

static bool matchPid(void *elem, void *data)
{
	ProcessADT proc = (ProcessADT) elem;
	uint64_t *targetPid = (uint64_t *) data;
	return proc->pid == *targetPid;
}

void addProcessInfo(uint64_t pid, char *name, uint8_t priority, void *basePointer, bool foreground)
{
	ProcessADT proc = FindInQueue(globalScheduler.queue, matchPid, &pid);

	if (proc)
	{
		proc->nombre = name;
		proc->priority = priority;
		proc->basePointer = basePointer;
		proc->foreground = foreground;
	}
}

void *schedule(void *stackPointer)
{
	if (globalScheduler.currentProcess)
	{
		globalScheduler.currentProcess->stack = stackPointer;

		if (globalScheduler.currentProcess != idleProcess)
		{
			if (--globalScheduler.currentProcess->quantumLeft)
			{
				return stackPointer;
			}
			Enqueue(globalScheduler.queue, globalScheduler.currentProcess);
		}
	}

	if (IsQueueEmpty(globalScheduler.queue))
	{
		globalScheduler.currentProcess = idleProcess;
		return idleProcess->stack;
	}

	globalScheduler.currentProcess = (ProcessADT) Dequeue(globalScheduler.queue);
	globalScheduler.currentProcess->quantumLeft = QUANTUM;
	return globalScheduler.currentProcess->stack;
}