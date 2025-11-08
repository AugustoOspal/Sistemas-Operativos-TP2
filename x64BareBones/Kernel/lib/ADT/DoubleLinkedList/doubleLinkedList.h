#ifndef SISTEMAS_OPERATIVOS_TP2_DOUBLELINKEDLIST_H
#define SISTEMAS_OPERATIVOS_TP2_DOUBLELINKEDLIST_H
#include "../../../mem/include/pmem.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct doubleLinkedListCDT *doubleLinkedListADT;

doubleLinkedListADT newDoubleLinkedListADT(void);
void FreeDoubleLinkedListCDT(doubleLinkedListADT list);
void addToDoubleLinkedList(doubleLinkedListADT list, void *elem);
void addToDoubleLinkedListSorted(doubleLinkedListADT list, void *elem, int (*compare)(void *a, void *b));
void *findInDoubleLinkedList(doubleLinkedListADT list, bool (*equals)(void *elem, void *target), void *target);
void removeFromDoubleLinkedList(doubleLinkedListADT list, bool (*equals)(void *elem, void *target), void *target);
size_t getDoubleLinkedListLength(doubleLinkedListADT list);
bool isListEmpty(doubleLinkedListADT list);

#endif // SISTEMAS_OPERATIVOS_TP2_DOUBLELINKEDLIST_H
