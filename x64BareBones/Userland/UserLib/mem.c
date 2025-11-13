#include "include/mem.h"
#include "include/syscallLib.h"

void *mem_alloc(const size_t size)
{
	return sys_mem_alloc(size);
}
void mem_free(void *ptr)
{
	sys_mem_free(ptr);
}

void get_mem_info(pm_stats_t *stats)
{
	sys_mem_info(stats);
}
