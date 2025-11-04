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
} nodeT;

typedef struct nodeT *nodeP;

QueueADT NewQueue(void)
{
	const QueueADT queue = mem_alloc(sizeof(QueueCDT));

	if (queue)
	{
		queue->head = NULL;
		queue->tail = NULL;
		queue->length = 0;
		return queue;
	}

	return NULL;
}

void FreeQueue(const QueueADT queue)
{
	if (queue)
	{
		nodeP current = queue->head;
		while (current != NULL)
		{
			const nodeP next = current->next;
			mem_free(current);
			current = next;
		}

		mem_free(queue);
	}
}

void Enqueue(const QueueADT queue, void *obj)
{
	if (!queue || !obj)
		return;

	// TODO: Validar memoria
	const nodeP newNode = mem_alloc(sizeof(nodeT));
	newNode->obj = obj;
	newNode->next = NULL;

	if (queue->tail == NULL)
	{
		queue->head = newNode;
		queue->tail = newNode;
	}
	else
	{
		queue->tail->next = newNode;
		queue->tail = newNode;
	}
	queue->length++;
}

void *Dequeue(const QueueADT queue)
{
	if (!queue || queue->length == 0)
		return NULL;

	const nodeP toRemove = queue->head;
	void *obj = toRemove->obj;

	queue->head = queue->head->next;
	if (queue->head == NULL)
	{
		queue->tail = NULL;
	}

	queue->length--;
	mem_free(toRemove);
	return obj;
}

int QueueLength(const QueueADT queue)
{
	if (!queue)
		return -1;
	return queue->length;
}

bool IsQueueEmpty(const QueueADT queue)
{
	// TODO: validar si la cola no existe
	if (!queue)
		return true;
	return !queue->length;
}

void *FindInQueue(const QueueADT queue, bool (*predicate)(void *elem, void *data), void *data)
{
	if (!queue)
		return NULL;

	nodeP current = queue->head;
	while (current != NULL)
	{
		if (predicate(current->obj, data))
		{
			return current->obj;
		}
		current = current->next;
	}

	return NULL;
}

void RemoveFromQueue(const QueueADT queue, void *obj)
{
	if (!queue || !obj)
		return;

	nodeP current = queue->head;
	nodeP previous = NULL;

	while (current != NULL)
	{
		if (current->obj == obj)
		{
			if (previous == NULL)
			{
				queue->head = current->next;
				if (queue->head == NULL)
				{
					queue->tail = NULL;
				}
			}
			else
			{
				previous->next = current->next;
				if (current->next == NULL)
				{
					queue->tail = previous;
				}
			}

			queue->length--;
			mem_free(current);
			return;
		}
		previous = current;
		current = current->next;
	}
}
