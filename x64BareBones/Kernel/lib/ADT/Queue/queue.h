#ifndef _QUEUE_H
#define _QUEUE_H

#include "../../../mem/include/pmem.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct QueueCDT *QueueADT;

QueueADT NewQueue(void);
void FreeQueue(QueueADT queue);
void Enqueue(QueueADT queue, void *obj);
void *Dequeue(QueueADT queue);
int QueueLength(QueueADT queue);
bool IsQueueEmpty(QueueADT queue);
void *FindInQueue(const QueueADT queue, bool (*equals)(void *elem, void *target), void *target);
void RemoveFromQueue(QueueADT queue, void *obj);
void RemoveFromQueueIf(QueueADT queue, bool (*equals)(void *elem, void *target), void *target);

#endif
