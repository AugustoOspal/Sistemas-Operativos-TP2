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
	if (!newProcess) return NULL;

	newProcess->pid = lastPid++;
	newProcess->stack = stackPointer;
	newProcess->basePointer = NULL;
    newProcess->nombre = NULL;

	newProcess->state = READY;
	newProcess->quantumLeft = QUANTUM;
	newProcess->priority = DEFAULT_PRIORITY;
	newProcess->foreground = false;

	newProcess->parent = globalScheduler.currentProcess;
    newProcess->childrenCount = 0;
    newProcess->reapedCount = 0;
    newProcess->waitingPid = (uint64_t)-1;

    addToDoubleLinkedList(globalScheduler.processTable, newProcess);
    Enqueue(globalScheduler.priorityQueues[DEFAULT_PRIORITY], newProcess);

    if (globalScheduler.currentProcess) {
        globalScheduler.currentProcess
            ->children[globalScheduler.currentProcess->childrenCount++] = newProcess->pid;
    }
    return newProcess;
}

static bool matchPid(void *elem, void *data)
{
	ProcessADT proc = (ProcessADT) elem;
	uint64_t *targetPid = (uint64_t *) data;
	return proc->pid == *targetPid;
}

void addProcessInfo(uint64_t pid, char *name, void *basePointer, bool foreground)
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
			if(globalScheduler.currentProcess->state == RUNNING){
				if (--globalScheduler.currentProcess->quantumLeft){
					return stackPointer;
				}else{
					globalScheduler.currentProcess->state = READY;
				}
			}
	        if (globalScheduler.currentProcess->state == READY) {
                uint8_t pr = globalScheduler.currentProcess->priority;
                Enqueue(globalScheduler.priorityQueues[pr], globalScheduler.currentProcess);
            }
		}
	}
	return pickNextProcess();
}

static ProcessADT getProcessByPid(uint64_t pid){
	return findInDoubleLinkedList(globalScheduler.processTable, matchPid, &pid);
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
	RemoveFromQueue(globalScheduler.priorityQueues[proc->priority], matchPid, proc->pid);
	proc->priority = newPriority;
	Enqueue(globalScheduler.priorityQueues[newPriority], proc);
	if(proc->state == RUNNING){
		globalScheduler.currentProcess = NULL;
		forceTimerInterrupt();
	}
}

static void deleteProcess(ProcessADT p){
	removeFromDoubleLinkedList(globalScheduler.processTable, matchPid, p);
	mem_free(p->stack);
	mem_free(p);
}

void terminateProcess(uint64_t pid){
	ProcessADT p = getProcessByPid(pid);
    if (!p) return;

	RemoveFromQueue(globalScheduler.priorityQueues[p->priority], matchPid, p->pid);
	RemoveFromQueue(globalScheduler.blockedQueue, matchPid, p->pid);

	//cambiar padre de hijos corriendo y finalizar zombies
	for(int i = 0; i < p->childrenCount; i++){
		ProcessADT childrenP = getProcessByPid(p->children[i]);
		childrenP->parent = NULL;
	}
	p->childrenCount = 0;

	for(int i = 0; i < p->reapedCount; i++){
		terminateProcess(p->reaped[i]);
	}
	p->reapedCount = 0;

	p->state = ZOMBIE;

	if(p->parent){
		p->parent->reaped[p->parent->reapedCount++] = p->pid;
		if ((p->parent->waitingPid == 0 || p->parent->waitingPid == p->pid) && p->parent->state == BLOCKED) {
			unblockProcess(p->parent->pid);
		}
	}else{	
		deleteProcess(p);
	}
	if (globalScheduler.currentProcess == p) {
        forceTimerInterrupt();
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

	RemoveFromQueue(globalScheduler.priorityQueues[p->priority], matchPid, p->pid);
	p->state = BLOCKED;

	Enqueue(globalScheduler.blockedQueue, p);
}

void unblockProcess(uint64_t pid){
	ProcessADT p = getProcessByPid(pid);
	if (!p) return;

	RemoveFromQueue(globalScheduler.blockedQueue,matchPid , p->pid);
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
	//busco proceso
	ProcessADT process = globalScheduler.currentProcess;

	//si lo que necesito no esta me bloqueo
	if (process->reapedCount == 0){
		process->waitingPid = 0; //espera a cualquiera
		blockProcess(process->pid);
	}

	//armar status con datos de waitingProcess

	//elimino proceso de reaped
	ProcessADT reapedProcess = getProcessByPid(process->reaped[--process->reapedCount]); //el siguiente lo pisa
	if (!reapedProcess) return -1;

	//elimino proceso zombie
	uint64_t pid = reapedProcess->pid;
	deleteProcess(reapedProcess);
	return pid;
}

uint64_t waitpid(uint64_t pid, void *status){
	//busco proceso
	ProcessADT waitingProcess = getProcessByPid(pid);
	if (!waitingProcess) return -1;
	ProcessADT parentProcess = globalScheduler.currentProcess;
	if (!parentProcess) return -1;

	//si lo que necesito no esta me bloqueo
	if(waitingProcess->state != ZOMBIE){
		globalScheduler.currentProcess->waitingPid = pid;
		blockProcess(globalScheduler.currentProcess->pid);
	}

	//armar status con datos de waitingProcess

	//elimino proceso de reaped
	for(int i = 0; i < parentProcess->reapedCount; i++){
		if(parentProcess->reaped[i] == pid){
			//lo elimino y corro todo hacia izquierda
			for(int j = i+1; j < parentProcess->reapedCount; j++){
				parentProcess->reaped[j-1] = parentProcess->reaped[j];
			}
			parentProcess->reaped[--parentProcess->reapedCount] = 0;
		}
	}

	//elimino proceso zombie
	deleteProcess(waitingProcess);
	return pid;
}


/*
estrategia wait y terminar procesos

Todos los procesos que mueren pasan a estado zombie (que implica no estar en cola de ejecucion, haber modificado el padre de los hijos, agregarse a reaped de padre, etc)
El padre hace wait y espera a que un hijo (o cualquiera) termine.
Si el proceso que busca no esta en reaped se bloquea.
El proceso que estaba esperando es quien lo desbloquea porque terminó -> se que esta zombie y en reaped
Agarro su informacion, lo saco de mi reaped y lo elimino con delete (elimina de pcb y libera)
*/