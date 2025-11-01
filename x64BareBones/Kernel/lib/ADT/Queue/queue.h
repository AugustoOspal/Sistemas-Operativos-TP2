#ifndef _QUEUE_H
#define _QUEUE_H

#include <stdbool.h>
#include <stddef.h>
#include "pmem.h"

typedef struct QueueCDT *QueueADT;

QueueADT NewQueue(void);
void FreeQueue(QueueADT queue);
void Enqueue(QueueADT queue, void *obj);
void *Dequeue(QueueADT queue);
int QueueLength(QueueADT queue);
bool IsQueueEmpty(QueueADT queue);

#endif