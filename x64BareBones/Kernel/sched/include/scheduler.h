#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "../ADT/DoubleLinkedList/doubleLinkedList.h"
#include "../ADT/Queue/queue.h"
#include "idle.h"
#include "pmem.h"
#include "process.h"
#include <stdbool.h>
#include <stdint.h>

#define QUANTUM 5

typedef struct ProcessCDT *ProcessADT;

void initializeScheduler();
void *schedule(void *stackPointer);
ProcessADT addProcess(void *stackPointer);
void addProcessInfo(uint64_t pid, char *name, uint8_t priority, void *basePointer, bool foreground);
// removeProcess();
// blockProcess();
// unblockProcess();
// getProcessInfo();
// changeProcessPriority();
// resignTimeWindow(); // usa yield

#endif
