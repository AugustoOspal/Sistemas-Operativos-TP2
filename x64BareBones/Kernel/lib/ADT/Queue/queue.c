#include "queue.h"

#include "../../../mem/include/pmem.h"

typedef struct QueueCDT
{
    struct nodeT *head;
    struct nodeT *tail;
    int length;
} QueueCDT;

typedef struct nodeT
{
    void *obj;
    struct nodeT *next;
}nodeT;

typedef struct nodeT *nodeP;

QueueADT NewQueue(void)
{
    QueueADT queue = mem_alloc(sizeof(QueueCDT));
    queue->head = NULL;
    queue->tail = NULL;
    queue->length = 0;
    return queue;
}

void FreeQueue(QueueADT queue)
{
    // TODO: Aca tendriamos que ver como manejar el error
    if (!queue || !queue->length)
    {
        return;
    }

    mem_free(queue);
}

void Enqueue(QueueADT queue, void *obj)
{
    nodeP newNode = mem_alloc(sizeof(nodeT));
    newNode->obj = obj;
    newNode->next = queue->tail;

    queue->tail = newNode;
    queue->length++;
}

void *Dequeue(QueueADT queue)
{
    if (queue->length == 0)
        return NULL;

    nodeP node = queue->tail;
    for (; node->next != queue->head; node = node->next);
    queue->head = node;

    nodeP tmp = node->next;
    node->next = NULL;
    return tmp->obj;
}

int QueueLength(QueueADT queue) {
    return queue->length;
}

bool IsQueueEmpty(QueueADT queue) {
    return !queue->length;
}