// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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

doubleLinkedListADT newDoubleLinkedListADT(void)
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

void addToDoubleLinkedListSorted(doubleLinkedListCDT *list, void *elem, int (*compare)(void *a, void *b))
{
	if (!list || !compare)
		return;

	node_t *node = mem_alloc(sizeof(node_t));
	if (!node)
		return;

	node->elem = elem;

	// Lista vacía - insertar como primer elemento
	if (!list->head)
	{
		node->prev = NULL;
		node->next = NULL;
		list->head = node;
		list->length++;
		return;
	}

	// Buscar posición de inserción
	node_t *cur = list->head;
	while (cur)
	{
		// Si elem < cur->elem, insertar antes de cur
		if (compare(elem, cur->elem) < 0)
		{
			node->next = cur;
			node->prev = cur->prev;

			if (cur->prev)
				cur->prev->next = node;
			else
				list->head = node; // Insertar al principio

			cur->prev = node;
			list->length++;
			return;
		}

		// Si es el último nodo, insertar al final
		if (!cur->next)
		{
			node->next = NULL;
			node->prev = cur;
			cur->next = node;
			list->length++;
			return;
		}

		cur = cur->next;
	}
}

void *findInDoubleLinkedList(doubleLinkedListADT list, bool (*equals)(void *elem, void *target), void *target)
{
	if (!list || !equals)
		return NULL;

	node_t *cur = list->head;
	while (cur)
	{
		if (equals(cur->elem, target))
		{
			return cur->elem;
		}
		cur = cur->next;
	}
	return NULL;
}

void removeFromDoubleLinkedList(doubleLinkedListADT list, bool (*equals)(void *elem, void *target), void *target)
{
	if (!list || !equals)
		return;

	node_t *cur = list->head;
	while (cur)
	{
		if (equals(cur->elem, target))
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
