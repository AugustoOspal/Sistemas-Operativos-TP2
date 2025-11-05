#include "scheduler.h"
#include "../lib/ADT/DoubleLinkedList/doubleLinkedList.h"

typedef enum ProcessState
{
	READY,
	RUNNING,
	BLOCKED,
	ZOMBIE
} ProcessState;

typedef struct ProcessCDT
{
	void *basePointer;
	void *stack;

	uint64_t pid;
	char *nombre;
	ProcessState state;
	uint8_t priority;

	ProcessADT parent;
	uint64_t children[MAX_CHILDREN];
	size_t childrenCount;
	uint64_t reaped[MAX_CHILDREN]; //recolectados
	size_t reapedCount;
	uint64_t waitingPid; //=-1 si no espera, =0 si espera a cualquiera, >0 matchea pid

	uint8_t quantumLeft;
	bool foreground;
} ProcessCDT;

typedef struct schedulerT
{
	ProcessADT currentProcess;
	doubleLinkedListADT processTable;
	QueueADT priorityQueues[PRIO];
	uint8_t   weights[PRIO];
    uint8_t   credits[PRIO];
	QueueADT blockedQueue;
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
	globalScheduler.processTable = newDoubleLinkedListCDT();
	for(int i = 0; i < PRIO; i++)
	{
		globalScheduler.priorityQueues[i] = NewQueue();
	}
	globalScheduler.currentProcess = NULL;

	//ver que si cambia PRIO no haya que cambiar el codigo -> hacerlo mejor
	for(int i = 0; i < PRIO; i++){
		globalScheduler.weights[i] = PRIO - i;
	}
    refill_credits();

	globalScheduler.blockedQueue = NewQueue();

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
	newProcess->parent = globalScheduler.currentProcess;

	addToDoubleLinkedList(globalScheduler.processTable, newProcess);
	Enqueue(globalScheduler.priorityQueues[DEFAULT_PRIORITY], newProcess);

	globalScheduler.currentProcess->children[globalScheduler.currentProcess->childrenCount++] = newProcess->pid;
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
	ProcessADT proc = getProcessByPid(pid);
	if (proc) {
		proc->nombre = name;
		proc->basePointer = basePointer;
		proc->foreground = foreground;
		return;
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

static ProcessADT getProcessByPid(uint64_t pid){
	return findInDoubleLinkedListIf(globalScheduler.processTable, matchPid, &pid);
}

void changeProcessPriority(uint64_t pid, uint8_t newPriority)
{
	ProcessADT proc = getProcessByPid(pid);
	if (newPriority >= PRIO || !proc || proc->priority == newPriority) return;
	if (proc->state == BLOCKED) {
		proc->priority = newPriority;
		return;
	}else if(proc->state == ZOMBIE){
		return;
	}
	RemoveFromQueue(globalScheduler.priorityQueues[proc->priority], proc);
	proc->priority = newPriority;
	Enqueue(globalScheduler.priorityQueues[newPriority], proc);
	if(proc->state == RUNNING){
		globalScheduler.currentProcess = NULL;
		forceTimerInterrupt();
	}
}

static void deleteProcess(ProcessADT p){
	removeFromDoubleLinkedList(globalScheduler.processTable, p);
	// liberar memoria del stack y del PCB
	mem_free(p->stack);
	mem_free(p);
	if(p->state == RUNNING){
		forceTimerInterrupt();
	}
}

void terminateProcess(uint64_t pid){
	ProcessADT p = getProcessByPid(pid);
    if (!p) return;

	// sacar de cualquier cola en la que este (si sigue en alguna)
	RemoveFromQueue(globalScheduler.priorityQueues[p->priority], p);

	if (p->parent->waitingPid == p->pid || p->parent->waitingPid == 0) { //el proceso padre estaba esperando
		deleteProcess(p);
		p->parent->reaped[--p->parent->reapedCount] = 0;
		if(p->parent->state == BLOCKED){
			unblockProcess(p->parent->pid);
		}
	} else { //el proceso padre no estaba esperando -> puede esperar despues
		p->parent->reaped[p->parent->reapedCount++] = p->pid;
		p->state = ZOMBIE;
	}
}

static inline void forceTimerInterrupt(void) { //hacer prolijo en asm
    __asm__ __volatile__ ("int $0x20");
}

void blockProcess(uint64_t pid){
	if (globalScheduler.currentProcess && globalScheduler.currentProcess->pid == pid){
		ProcessADT running = globalScheduler.currentProcess;
		if (running->state == BLOCKED)
			return;
		running->state = BLOCKED;
		Enqueue(globalScheduler.blockedQueue, running);
		forceTimerInterrupt();
		return;
	}

	ProcessADT p = getProcessByPid(pid);
	if (!p) return;

	RemoveFromQueue(globalScheduler.priorityQueues[p->priority], p);
	p->state = BLOCKED;

	Enqueue(globalScheduler.blockedQueue, p);
}

void unblockProcess(uint64_t pid){
	ProcessADT p = getProcessByPid(pid);
	if (!p) return;

	RemoveFromQueue(globalScheduler.blockedQueue, p);
	p->state = READY;

	Enqueue(globalScheduler.priorityQueues[p->priority], p);
}

void yield() {
    globalScheduler.currentProcess->quantumLeft = 1;
    forceTimerInterrupt();
}

uint64_t getPid(){
	return globalScheduler.currentProcess->pid;
}

void kill(uint64_t pid){
	terminateProcess(pid);
}

uint64_t wait(void *status){
	ProcessADT process = globalScheduler.currentProcess;

	if (process->reapedCount > 0){
		ProcessADT reapedProcess = getProcessByPid(process->reaped[--process->reapedCount]);
		if (!reapedProcess) return -1;

		deleteProcess(reapedProcess);
		return reapedProcess->pid;
	} else {
		process->waitingPid = 0; //espera a cualquiera
		blockProcess(process->pid);
		return -1;
	}
}

uint64_t waitpid(uint64_t pid, void *status){
	ProcessADT waitingProcess = getProcessByPid(pid);
	if (!waitingProcess) return -1;

	if (waitingProcess->state == ZOMBIE){
		//armar status con datos de waitingProcess
		deleteProcess(waitingProcess);
		return pid;
	} else {
		globalScheduler.currentProcess->waitingPid = pid;
		blockProcess(globalScheduler.currentProcess->pid);
		return -1;
	}
}