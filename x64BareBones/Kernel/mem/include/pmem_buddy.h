#ifndef PMEM_BUDDY_H
#define PMEM_BUDDY_H
#include <stddef.h>
#include <stdint.h>

#define BUDDY_MIN_ORDER 12 // 2^12 = 4096 bytes
#define BUDDY_MAX_ORDER 32 // hasta 2^32 bytes (4 GB)

void  buddy_init(void *base, size_t length);
void *buddy_alloc(size_t size);
void  buddy_free(void *ptr);
#endif