#include "scheduler.h"
#include "../ipc/include/pipe.h"
#include "../lib/ADT/DoubleLinkedList/doubleLinkedList.h"
#include "../lib/string/strings.h"
#include <stdint.h>
#include "interrupts.h"

#define PID_COL_WIDTH 5
#define NAME_COL_WIDTH 16
#define PRIO_COL_WIDTH 8
#define STATE_COL_WIDTH 8
#define FG_COL_WIDTH 10
#define ADDR_COL_WIDTH 12
#define PROCESS_INFO_LINE_MAX 160

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
	char **argv;

	uint64_t pid;
	char *nombre;
	ProcessState state;
	uint8_t priority;

	ProcessADT parent;
	doubleLinkedListADT children;  // Lista de PIDs (uint64_t cast a void*)
	doubleLinkedListADT reaped;    // Lista de PIDs (uint64_t cast a void*)
	uint64_t waitingPid; //=-1 si no espera, =0 si espera a cualquiera, >0 matchea pid

	int16_t fileDescriptors[FD_AMOUNT]; // espacio para 20 descriptores de archivo abiertos

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
static bool matchPidInList(void *elem, void *data);
static bool alwaysTrue(void *elem, void *data);
static ProcessADT getProcessByPid(uint64_t pid);
static int pickNextQueue(void);
static void *pickNextProcess();
static void deleteProcess(ProcessADT p);
static const char *getProcessStateString(ProcessState state);

static void refill_credits(void)
{
	for (int i = 0; i < PRIO; i++)
		globalScheduler.credits[i] = globalScheduler.weights[i];
}

static const char *getProcessStateString(ProcessState state)
{
	switch (state)
	{
		case READY:
			return "READY";
		case RUNNING:
			return "RUNNING";
		case BLOCKED:
			return "BLOCKED";
		case ZOMBIE:
			return "ZOMBIE";
		default:
			return "UNKNOWN";
	}
}

void initializeScheduler()
{
	globalScheduler.processTable = newDoubleLinkedListADT();
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

	int fds[] = {STDIN, STDOUT, STDERR};
	createProcess("idle", idleMain, 0, NULL, fds, false); // idle nunca es foreground
	idleProcess = Dequeue(globalScheduler.priorityQueues[DEFAULT_PRIORITY]);
}

uint64_t addProcess(void *stackPointer, const int16_t fds[FD_AMOUNT])
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
	newProcess->argv = NULL;

	newProcess->state = READY;
	newProcess->quantumLeft = QUANTUM;
	newProcess->priority = DEFAULT_PRIORITY;
	newProcess->foreground = false;
	newProcess->fileDescriptors[0] = fds[0];
	newProcess->fileDescriptors[1] = fds[1];
	newProcess->fileDescriptors[2] = fds[2];

	newProcess->parent = globalScheduler.currentProcess;
	newProcess->children = newDoubleLinkedListADT();
	newProcess->reaped = newDoubleLinkedListADT();
	newProcess->waitingPid = (uint64_t) -1;

	addToDoubleLinkedList(globalScheduler.processTable, newProcess);
	Enqueue(globalScheduler.priorityQueues[DEFAULT_PRIORITY], newProcess);

	// Agregar a la lista de hijos del padre
	if (globalScheduler.currentProcess)
	{
		addToDoubleLinkedList(globalScheduler.currentProcess->children, (void *) (uintptr_t) newProcess->pid);
	}
	return newProcess->pid;
}

static bool matchPid(void *elem, void *data)
{
	ProcessADT proc = (ProcessADT) elem;
	uint64_t *targetPid = (uint64_t *) data;
	return proc->pid == *targetPid;
}

// Helper para comparar PIDs almacenados como void* en listas
static bool matchPidInList(void *elem, void *data)
{
	uint64_t elemPid = (uint64_t) (uintptr_t) elem;
	const uint64_t *targetPid = (const uint64_t *) data;
	return elemPid == *targetPid;
}

// Helper que siempre retorna true (para obtener cualquier elemento de la lista)
static bool alwaysTrue(void *elem, void *data)
{
	(void) elem;
	(void) data;
	return true;
}

static ProcessADT getProcessByPid(uint64_t pid)
{
	return findInDoubleLinkedList(globalScheduler.processTable, matchPid, &pid);
}

void addProcessInfo(uint64_t pid, const char *name, void *basePointer, char **argv, bool foreground)
{
	ProcessADT proc = getProcessByPid(pid);
	if (proc)
	{
		proc->nombre = name;
		proc->basePointer = basePointer;
		proc->argv = argv;
		proc->foreground = foreground;
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
	removeFromDoubleLinkedList(globalScheduler.processTable, matchPid, &p->pid);
	if (p->basePointer)
	{
		mem_free(p->basePointer);
	}

	if (p->argv)
	{
		for (int i = 0; p->argv[i] != NULL; i++)
		{
			mem_free(p->argv[i]);
		}
		mem_free(p->argv);
	}

	// Liberar listas de hijos y zombies
	FreeDoubleLinkedListCDT(p->children);
	FreeDoubleLinkedListCDT(p->reaped);

	mem_free(p);
}

void terminateProcess(uint64_t pid)
{
	ProcessADT p = getProcessByPid(pid);
	if (!p)
		return;

	RemoveFromQueue(globalScheduler.priorityQueues[p->priority], matchPid, &p->pid);
	RemoveFromQueue(globalScheduler.blockedQueue, matchPid, &p->pid);

	// Cambiar padre de hijos corriendo (orphan running children)
	while (!isListEmpty(p->children))
	{
		uint64_t childPid = (uint64_t) findInDoubleLinkedList(p->children, alwaysTrue, NULL);
		ProcessADT childProc = getProcessByPid(childPid);
		if (childProc)
		{
			childProc->parent = NULL;
		}
		removeFromDoubleLinkedList(p->children, matchPidInList, &childPid);
	}

	// Finalizar todos los procesos zombies
	while (!isListEmpty(p->reaped))
	{
		uint64_t reapedPid = (uint64_t) findInDoubleLinkedList(p->reaped, alwaysTrue, NULL);
		ProcessADT zombieProc = getProcessByPid(reapedPid);
		removeFromDoubleLinkedList(p->reaped, matchPidInList, &reapedPid);
		if (zombieProc)
		{
			deleteProcess(zombieProc);  // Borrar directamente, ya es zombie
		}
	}

	p->state = ZOMBIE;

	// Agregar a la lista de reaped del padre
	if (p->parent)
	{
		addToDoubleLinkedList(p->parent->reaped, (void *) p->pid);
		if ((p->parent->waitingPid == 0 || p->parent->waitingPid == p->pid) && p->parent->state == BLOCKED)
		{
			unblockProcess(p->parent->pid);
		}
	}
	else
	{
		// Sin padre -> borrar directamente
		deleteProcess(p);
	}

	if (globalScheduler.currentProcess == p)
	{
		_timerInterrupt();
	}
}

void addProcessToBlockQueue(uint64_t pid)
{
	if (globalScheduler.currentProcess && globalScheduler.currentProcess->pid == pid)
	{
		ProcessADT running = globalScheduler.currentProcess;
		if (running->state == BLOCKED)
			return;
		running->state = BLOCKED;
		Enqueue(globalScheduler.blockedQueue, running);
		return;
	}

	ProcessADT p = getProcessByPid(pid);
	if (!p)
		return;

	RemoveFromQueue(globalScheduler.priorityQueues[p->priority], matchPid, &p->pid);
	p->state = BLOCKED;

	Enqueue(globalScheduler.blockedQueue, p);
}

void blockProcess(uint64_t pid)
{
	addProcessToBlockQueue(pid);
	if (pid == globalScheduler.currentProcess->pid)
	{
		_timerInterrupt();
	}
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

	const char *state_str = getProcessStateString(p->state);

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
	(void) status;  // Unused parameter
	ProcessADT process = globalScheduler.currentProcess;

	// Si no hay procesos zombies, bloquear
	if (isListEmpty(process->reaped))
	{
		process->waitingPid = 0; // espera a cualquiera
		blockProcess(process->pid);
	}

	// Obtener el primer proceso zombie (any one)
	uint64_t reapedPid = (uint64_t) findInDoubleLinkedList(process->reaped, alwaysTrue, NULL);
	ProcessADT reapedProcess = getProcessByPid(reapedPid);
	if (!reapedProcess)
		return -1;

	// Remover de lista de reaped
	removeFromDoubleLinkedList(process->reaped, matchPidInList, &reapedPid);

	// Eliminar proceso zombie
	deleteProcess(reapedProcess);
	return reapedPid;
}

uint64_t waitPid(uint64_t pid)
{
	ProcessADT waitingProcess = getProcessByPid(pid);
	if (!waitingProcess)
		return -1;
	ProcessADT parentProcess = globalScheduler.currentProcess;
	if (!parentProcess)
		return -1;

	// Si el proceso no es zombie todavía, bloquear
	if (waitingProcess->state != ZOMBIE)
	{
		globalScheduler.currentProcess->waitingPid = pid;
		blockProcess(globalScheduler.currentProcess->pid);
	}

	// Remover de la lista de reaped del padre
	removeFromDoubleLinkedList(parentProcess->reaped, matchPidInList, &pid);

	// Eliminar proceso zombie
	deleteProcess(waitingProcess);
	return pid;
}

static bool matchForeground(void *elem, void *data)
{
	ProcessADT proc = (ProcessADT) elem;
	return proc->foreground && proc->state != ZOMBIE;
}

uint64_t getForegroundPid(void)
{
	ProcessADT fgProcess = findInDoubleLinkedList(globalScheduler.processTable, matchForeground, NULL);
	return fgProcess ? fgProcess->pid : 0;
}

int16_t getProcesFd(const int fdIdx)
{
	return globalScheduler.currentProcess->fileDescriptors[fdIdx];
}

int64_t getProcessStatus(uint64_t pid)
{
	ProcessADT proc = getProcessByPid(pid);
	if (!proc)
		return -1;

	return (int64_t) proc->state;
}

/*
estrategia wait y terminar procesos

Todos los procesos que mueren pasan a estado zombie (que implica no estar en cola de ejecucion, haber modificado el
padre de los hijos, agregarse a reaped de padre, etc) El padre hace wait y espera a que un hijo (o cualquiera) termine.
Si el proceso que busca no esta en reaped se bloquea.
El proceso que estaba esperando es quien lo desbloquea porque terminó -> se que esta zombie y en reaped
Agarro su informacion, lo saco de mi reaped y lo elimino con delete (elimina de pcb y libera)
*/
