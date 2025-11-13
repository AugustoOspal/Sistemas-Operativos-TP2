#ifndef MEM_H
#define MEM_H
#include "syscallLib.h"
#include <stddef.h>

void *mem_alloc(size_t size);
void mem_free(void *ptr);
;
void get_mem_info(pm_stats_t *stats);

#endif