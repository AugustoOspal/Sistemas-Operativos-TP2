#pragma once
#include <stdint.h>
#include <stddef.h>

#define PM_PAGE_SIZE 4096

typedef struct {
    uint64_t total;
    uint64_t used;
    uint64_t free;
} pm_stats_t;

void  pm_init(void *pool_start, size_t pool_len);
void *pm_alloc(size_t bytes);
void  pm_free(void *ptr);
void  pm_get_stats(pm_stats_t *out);
