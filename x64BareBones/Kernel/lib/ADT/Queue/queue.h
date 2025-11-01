#ifndef _QUEUE_H
#define _QUEUE_H

#include <stdbool.h>
#include <stddef.h>
#include "../../../mem/include/pmem.h"

typedef struct QueueCDT *QueueADT;

QueueADT NewQueue(void);
void FreeQueue(QueueADT queue);
void Enqueue(const QueueADT queue, void *obj);
void *Dequeue(const QueueADT queue);
int QueueLength(const QueueADT queue);
bool IsQueueEmpty(const QueueADT queue);

#endif