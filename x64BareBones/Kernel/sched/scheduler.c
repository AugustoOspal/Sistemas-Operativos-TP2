#include "scheduler.h"
#include "../lib/ADT/DoubleLinkedList/doubleLinkedList.h"
#include "../lib/string/strings.h"
#include "interrupts.h"

	typedef enum ProcessState {
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
	uint64_t reaped[MAX_CHILDREN]; // recolectados
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
	uint8_t weights[PRIO];
	uint8_t credits[PRIO];
	QueueADT blockedQueue;
} schedulerT;

schedulerT globalScheduler;
ProcessADT idleProcess;		 // Esta también puede ir adentro del schedulerT
static uint64_t lastPid = 0; // Este PID es el proximo a crear

static void refill_credits(void);
static bool matchPid(void *elem, void *data);
static ProcessADT getProcessByPid(uint64_t pid);
static int pickNextQueue(void);
static void *pickNextProcess();
static void deleteProcess(ProcessADT p);

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

	// No podria dar 0 porque esta reservado para el idle
	if (!newProcess)
		return 0;

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
	newProcess->waitingPid = (uint64_t) -1;

	addToDoubleLinkedList(globalScheduler.processTable, newProcess);
	Enqueue(globalScheduler.priorityQueues[DEFAULT_PRIORITY], newProcess);

	if (globalScheduler.currentProcess)
	{
		globalScheduler.currentProcess->children[globalScheduler.currentProcess->childrenCount++] = newProcess->pid;
	}
	return newProcess->pid;
}

static bool matchPid(void *elem, void *data)
{
	ProcessADT proc = (ProcessADT) elem;
	uint64_t *targetPid = (uint64_t *) data;
	return proc->pid == *targetPid;
}

static ProcessADT getProcessByPid(uint64_t pid)
{
	return findInDoubleLinkedList(globalScheduler.processTable, matchPid, &pid);
}

void addProcessInfo(uint64_t pid, const char *name, void *basePointer)
{
	ProcessADT proc = getProcessByPid(pid);
	if (proc)
	{
		proc->nombre = name;
		proc->basePointer = basePointer;
		proc->foreground = DEFAULT_FOREGROUND;
		return;
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
			if (globalScheduler.currentProcess->state == RUNNING)
			{
				if (--globalScheduler.currentProcess->quantumLeft)
				{
					return stackPointer;
				}
				else
				{
					globalScheduler.currentProcess->state = READY;
				}
			}
			if (globalScheduler.currentProcess->state == READY)
			{
				uint8_t pr = globalScheduler.currentProcess->priority;
				Enqueue(globalScheduler.priorityQueues[pr], globalScheduler.currentProcess);
			}
		}
	}
	return pickNextProcess();
}

void changeProcessPriority(uint64_t pid, uint8_t newPriority)
{
	ProcessADT proc = getProcessByPid(pid);
	if (newPriority >= PRIO || !proc || proc->priority == newPriority)
		return;
	if (proc->state == BLOCKED)
	{
		proc->priority = newPriority;
		return;
	}
	else if (proc->state == ZOMBIE)
	{
		return;
	}
	RemoveFromQueue(globalScheduler.priorityQueues[proc->priority], matchPid, &proc->pid);
	proc->priority = newPriority;
	Enqueue(globalScheduler.priorityQueues[newPriority], proc);
	if (proc->state == RUNNING)
	{
		globalScheduler.currentProcess = NULL;
		_timerInterrupt();
	}
}

static void deleteProcess(ProcessADT p)
{
	removeFromDoubleLinkedList(globalScheduler.processTable, matchPid, p);
	mem_free(p->stack);
	mem_free(p);
}

void terminateProcess(uint64_t pid)
{
	ProcessADT p = getProcessByPid(pid);
	if (!p)
		return;

	RemoveFromQueue(globalScheduler.priorityQueues[p->priority], matchPid, &p->pid);
	RemoveFromQueue(globalScheduler.blockedQueue, matchPid, &p->pid);

	// cambiar padre de hijos corriendo y finalizar zombies
	for (int i = 0; i < p->childrenCount; i++)
	{
		ProcessADT childrenP = getProcessByPid(p->children[i]);
		childrenP->parent = NULL;
	}
	p->childrenCount = 0;

	for (int i = 0; i < p->reapedCount; i++)
	{
		terminateProcess(p->reaped[i]);
	}
	p->reapedCount = 0;

	p->state = ZOMBIE;

	if (p->parent)
	{
		p->parent->reaped[p->parent->reapedCount++] = p->pid;
		if ((p->parent->waitingPid == 0 || p->parent->waitingPid == p->pid) && p->parent->state == BLOCKED)
		{
			unblockProcess(p->parent->pid);
		}
	}
	else
	{
		deleteProcess(p);
	}
	if (globalScheduler.currentProcess == p)
	{
		_timerInterrupt();
	}
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
		_timerInterrupt();
		return;
	}

	ProcessADT p = getProcessByPid(pid);
	if (!p)
		return;

	RemoveFromQueue(globalScheduler.priorityQueues[p->priority], matchPid, &p->pid);
	p->state = BLOCKED;

	Enqueue(globalScheduler.blockedQueue, p);
}

void unblockProcess(uint64_t pid)
{
	ProcessADT p = getProcessByPid(pid);
	if (!p)
		return;

	RemoveFromQueue(globalScheduler.blockedQueue, matchPid, &p->pid);
	p->state = READY;

	Enqueue(globalScheduler.priorityQueues[p->priority], p);
}

void yield()
{
	globalScheduler.currentProcess->quantumLeft = 1;
	_timerInterrupt();
}

uint64_t getPid()
{
	return globalScheduler.currentProcess->pid;
}

int getProcessInfo(uint64_t pid, char *buffer, uint64_t bufferSize)
{
	if (!buffer || bufferSize == 0)
		return -1;

	ProcessADT p = getProcessByPid(pid);

	if (!p)
		return -1;

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
		case ZOMBIE:
			state_str = "ZOMBIE";
			break;
		default:
			state_str = "UNKNOWN";
			break;
	}

	// Formato CSV: PID,Name,Priority,State,Foreground,Stack,BasePointer
	int written =
		ksprintf(buffer, "%lu,%s,%u,%s,%s,0x%lx,0x%lx", p->pid, p->nombre ? p->nombre : "(unnamed)", p->priority,
				 state_str, p->foreground ? "Yes" : "No", (unsigned long) p->stack, (unsigned long) p->basePointer);

	return written;
}

uint64_t getAllProcessesInfo(char *buffer, uint64_t bufferSize)
{
	if (!buffer || bufferSize == 0)
		return 0;

	// Escribir header CSV
	int pos = ksprintf(buffer, "PID,Name,Priority,State,Foreground,Stack,BasePointer\n");

	if (pos >= bufferSize)
	{
		buffer[bufferSize - 1] = '\0';
		return 0;
	}

	// TODO: A lo mejor en vez de contarlo asi se podria poner un campo en la struct
	uint64_t count = 0;
	for (uint64_t pid = 0; pid < lastPid; pid++)
	{
		ProcessADT p = getProcessByPid(pid);
		if (!p)
			continue;

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
			case ZOMBIE:
				state_str = "ZOMBIE";
				break;
			default:
				state_str = "UNKNOWN";
				break;
		}

		// Escribir directamente al buffer con ksprintf
		int written = ksprintf(buffer + pos, "%lu,%s,%u,%s,%s,0x%lx,0x%lx\n", p->pid,
							   p->nombre ? p->nombre : "(unnamed)", p->priority, state_str,
							   p->foreground ? "Yes" : "No", (unsigned long) p->stack, (unsigned long) p->basePointer);

		if (written > 0 && pos + written < bufferSize)
		{
			pos += written;
			count++;
		}
		else
		{
			break; // No hay más espacio
		}
	}

	// Null terminator
	if (pos < bufferSize)
		buffer[pos] = '\0';
	else
		buffer[bufferSize - 1] = '\0';

	return count;
}

void kill(uint64_t pid)
{
	terminateProcess(pid);
}

uint64_t wait(void *status)
{
	// busco proceso
	ProcessADT process = globalScheduler.currentProcess;

	// si lo que necesito no esta me bloqueo
	if (process->reapedCount == 0)
	{
		process->waitingPid = 0; // espera a cualquiera
		blockProcess(process->pid);
	}

	// armar status con datos de waitingProcess

	// elimino proceso de reaped
	ProcessADT reapedProcess = getProcessByPid(process->reaped[--process->reapedCount]); // el siguiente lo pisa
	if (!reapedProcess)
		return -1;

	// elimino proceso zombie
	uint64_t pid = reapedProcess->pid;
	deleteProcess(reapedProcess);
	return pid;
}

uint64_t waitpid(uint64_t pid, void *status)
{
	// busco proceso
	ProcessADT waitingProcess = getProcessByPid(pid);
	if (!waitingProcess)
		return -1;
	ProcessADT parentProcess = globalScheduler.currentProcess;
	if (!parentProcess)
		return -1;

	// si lo que necesito no esta me bloqueo
	if (waitingProcess->state != ZOMBIE)
	{
		globalScheduler.currentProcess->waitingPid = pid;
		blockProcess(globalScheduler.currentProcess->pid);
	}

	// armar status con datos de waitingProcess

	// elimino proceso de reaped
	for (int i = 0; i < parentProcess->reapedCount; i++)
	{
		if (parentProcess->reaped[i] == pid)
		{
			// lo elimino y corro todo hacia izquierda
			for (int j = i + 1; j < parentProcess->reapedCount; j++)
			{
				parentProcess->reaped[j - 1] = parentProcess->reaped[j];
			}
			parentProcess->reaped[--parentProcess->reapedCount] = 0;
		}
	}

	// elimino proceso zombie
	deleteProcess(waitingProcess);
	return pid;
}

/*
estrategia wait y terminar procesos

Todos los procesos que mueren pasan a estado zombie (que implica no estar en cola de ejecucion, haber modificado el
padre de los hijos, agregarse a reaped de padre, etc) El padre hace wait y espera a que un hijo (o cualquiera) termine.
Si el proceso que busca no esta en reaped se bloquea.
El proceso que estaba esperando es quien lo desbloquea porque terminó -> se que esta zombie y en reaped
Agarro su informacion, lo saco de mi reaped y lo elimino con delete (elimina de pcb y libera)
*/