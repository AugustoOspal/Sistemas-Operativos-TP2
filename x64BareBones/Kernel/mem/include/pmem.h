/*
 * Administra paginas fisicas, de ahi el nombre pm (physical memory)
 */

#pragma once
#include <stddef.h>
#include <stdint.h>

#define PM_PAGE_SIZE 4096

typedef struct
{
	uint64_t total;
	uint64_t used;
	uint64_t free;
} pm_stats_t;

int pm_init(void *pool_start, size_t pool_len);
void *mem_alloc(size_t bytes);
void mem_free(void *ptr);
void mem_get_stats(pm_stats_t *out);
