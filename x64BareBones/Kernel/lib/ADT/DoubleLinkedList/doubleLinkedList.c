//
// Created by augusto on 11/1/25.
//

#include "doubleLinkedList.h"

typedef struct nodeT
{
	void *elem;
	struct nodeT *next;
	struct nodeT *prev;
} node_t;

typedef struct doubleLinkedListCDT
{
	struct nodeT *head;
	int length;
} doubleLinkedListCDT;

doubleLinkedListCDT *newDoubleLinkedListCDT(void)
{
	doubleLinkedListCDT *list = mem_alloc(sizeof(doubleLinkedListCDT));
	if (!list)
		return NULL;
	list->head = NULL;
	list->length = 0;
	return list;
}

void FreeDoubleLinkedListCDT(doubleLinkedListADT list)
{
	if (!list || !list->length)
	{
		return;
	}
	node_t *cur = list->head;
	while (cur)
	{
		node_t *next = cur->next;

		mem_free(cur);
		cur = next;
	}
	mem_free(list);
}

void addToDoubleLinkedList(doubleLinkedListCDT *list, void *elem)
{
	if (!list)
		return;

	node_t *node = mem_alloc(sizeof(node_t));
	if (!node)
		return;

	node->elem = elem;
	node->prev = NULL;
	node->next = list->head;

	if (list->head)
		list->head->prev = node;

	list->head = node;
	list->length++;
}

void removeFromDoubleLinkedList(doubleLinkedListCDT *list, void *elem)
{
	if (!list || !list->head)
		return;

	node_t *cur = list->head;
	while (cur)
	{
		if (cur->elem == elem)
		{
			if (cur->prev)
			{
				cur->prev->next = cur->next;
			}
			else
			{
				list->head = cur->next;
			}
			if (cur->next)
			{
				cur->next->prev = cur->prev;
			}

			mem_free(cur);
			list->length--;
			return;
		}
		cur = cur->next;
	}
}

size_t getDoubleLinkedListLength(doubleLinkedListADT list)
{
	return list->length;
}

bool isListEmpty(doubleLinkedListADT list)
{
	return list->length == 0;
}