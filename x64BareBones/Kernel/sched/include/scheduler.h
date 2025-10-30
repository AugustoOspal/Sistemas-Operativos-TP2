#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include "process.h"

void scheduler_init(void);
int  scheduler_add(proc_t *p); //agrega un proceso a cola, devuelve 0 si ok
void scheduler_on_tick(uint64_t *current_rsp_addr);

#endif // SCHEDULER_H