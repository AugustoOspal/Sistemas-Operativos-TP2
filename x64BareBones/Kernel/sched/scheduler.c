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

typedef struct schedulerT
{
	QueueADT priorityQueues[PRIO];
	ProcessADT currentProcess;
	uint8_t   weights[PRIO];
    uint8_t   credits[PRIO];
} schedulerT;

schedulerT globalScheduler;

// A lo mejor este podria ir adentro de schedulerT
ProcessADT idleProcess;

// Este PID es el proximo a crear
static uint64_t lastPid = 0;

static void refill_credits(void) {
    for (int i = 0; i < PRIO; i++) globalScheduler.credits[i] = globalScheduler.weights[i];
}

void initializeScheduler()
{	
	for(int i = 0; i < PRIO; i++)
	{
		globalScheduler.priorityQueues[i] = NewQueue();
	}
	globalScheduler.currentProcess = NULL;

	//ver que si cambia PRIO no haya que cambiar el codigo -> hacerlo mejor
    globalScheduler.weights[0] = 3;
    globalScheduler.weights[1] = 2;
    globalScheduler.weights[2] = 1;
    refill_credits();

	createProcess(idleMain, 0, NULL);
	idleProcess = Dequeue(globalScheduler.priorityQueues[DEFAULT_PRIORITY]);
}

ProcessADT addProcess(void *stackPointer)
{
	// No estaria chequeando que existe scheduler

	ProcessADT newProcess = mem_alloc(sizeof(ProcessCDT));
	newProcess->pid = lastPid++;
	newProcess->stack = stackPointer;
	newProcess->state = READY;
	newProcess->quantumLeft = QUANTUM;
	newProcess->priority = DEFAULT_PRIORITY;
	Enqueue(globalScheduler.priorityQueues[DEFAULT_PRIORITY], newProcess);
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
			proc->basePointer = basePointer;
			proc->foreground = foreground;
			return;
		}
	}
}

static int pickNextQueue(void) {
    int sum = 0;
    for (int i = 0; i < PRIO; i++)
        if (!IsQueueEmpty(globalScheduler.priorityQueues[i]))
            sum += globalScheduler.credits[i];

    if (sum == 0) refill_credits();

    for (int p = 0; p < PRIO; p++){
        if (globalScheduler.credits[p] > 0 &&
            !IsQueueEmpty(globalScheduler.priorityQueues[p])){
            globalScheduler.credits[p]--;
            return p;
        }
    }
    return -1;
}

static void *pickNextProcess()
{
	int queue = pickNextQueue();
	if (queue < 0) {
        globalScheduler.currentProcess = idleProcess;
        return idleProcess->stack;
    }
	globalScheduler.currentProcess = (ProcessADT) Dequeue(globalScheduler.priorityQueues[queue]);
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
			uint8_t pr = globalScheduler.currentProcess->priority;
			Enqueue(globalScheduler.priorityQueues[pr], globalScheduler.currentProcess);
		}
	}

	return pickNextProcess();
}

void changeProcessPriority(uint64_t pid, uint8_t newPriority)
{
	for(int i = 0; i < PRIO; i++){
		ProcessADT proc = FindInQueue(globalScheduler.priorityQueues[i], matchPid, &pid);

		if (proc) {
			proc->priority = newPriority;
			Dequeue(globalScheduler.priorityQueues[i]);
			Enqueue(globalScheduler.priorityQueues[newPriority], proc);
			return;
		}
	}
}