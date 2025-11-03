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
	QueueADT priorityQueues[PRIO];
	ProcessADT currentProcess;
} schedulerT;

schedulerT globalScheduler;

// A lo mejor este podria ir adentro de schedulerT
ProcessADT idleProcess;

// Este PID es el proximo a crear
static uint64_t lastPid = 0;

void initializeScheduler()
{	
	for(int i = 0; i < 1; i++)
	{
		globalScheduler.priorityQueues[i] = NewQueue();
	}
	globalScheduler.currentProcess = NULL;
	createProcess(idleMain, 0, NULL);
	idleProcess = Dequeue(globalScheduler.priorityQueues[0]);
}

ProcessADT addProcess(void *stackPointer)
{
	// No estaria chequeando que existe scheduler

	ProcessADT newProcess = mem_alloc(sizeof(ProcessCDT));
	newProcess->pid = lastPid++;
	newProcess->stack = stackPointer;
	newProcess->state = READY;
	newProcess->quantumLeft = QUANTUM;
	Enqueue(globalScheduler.priorityQueues[0], newProcess);
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
	for(int i = 0; i < PRIO; i++){
		ProcessADT proc = FindInQueue(globalScheduler.priorityQueues[i], matchPid, &pid);

		if (proc) {
			proc->nombre = name;
			proc->priority = priority;
			proc->basePointer = basePointer;
			proc->foreground = foreground;
			return;
		}
	}
}

static int anyReadyProcess(){ 
	for(int i = 0; i < PRIO; i++){
		if (!IsQueueEmpty(globalScheduler.priorityQueues[i])){ return 1; }
	}
	return 0;
}

static void *pickNextProcess()
{
	if (!anyReadyProcess())
	{
		globalScheduler.currentProcess = idleProcess;
		return idleProcess->stack;
	}

	globalScheduler.currentProcess = (ProcessADT) Dequeue(globalScheduler.priorityQueues[0]);
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
			if (--globalScheduler.currentProcess->quantumLeft)
			{
				return stackPointer;
			}
			Enqueue(globalScheduler.priorityQueues[0], globalScheduler.currentProcess);
		}
	}

	return pickNextProcess();
}