#ifndef SISTEMAS_OPERATIVOS_TP2_DOUBLELINKEDLIST_H
#define SISTEMAS_OPERATIVOS_TP2_DOUBLELINKEDLIST_H
#include "../../../mem/include/pmem.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct doubleLinkedListCDT *doubleLinkedListADT;

doubleLinkedListADT newDoubleLinkedListCDT(void);
void FreeDoubleLinkedListCDT(doubleLinkedListADT list);
void addToDoubleLinkedList(doubleLinkedListADT list, void *elem);
void removeFromDoubleLinkedList(doubleLinkedListADT list, void *elem);
size_t getDoubleLinkedListLength(doubleLinkedListADT list);
bool isListEmpty(doubleLinkedListADT list);

#endif // SISTEMAS_OPERATIVOS_TP2_DOUBLELINKEDLIST_H