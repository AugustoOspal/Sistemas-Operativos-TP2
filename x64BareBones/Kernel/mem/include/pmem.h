#ifndef PMEM_H
#define PMEM_H
#include <stddef.h>
#include <stdint.h>

#define PM_PAGE_SIZE 4096
#define BUDDY_MIN_ORDER 12 // 2^12 = 4096 bytes
#define BUDDY_MAX_ORDER 32 // hasta 2^32 bytes (4 GB)

typedef struct
{
	uint64_t total;
	uint64_t used;
	uint64_t free;
} pm_stats_t;

int mem_init(void *base, size_t length);
void *mem_alloc(size_t size);
void mem_free(void *ptr);
void mem_get_stats(pm_stats_t *out);
#endif