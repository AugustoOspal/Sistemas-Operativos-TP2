#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include "process.h"

void scheduler_init(void);
int  scheduler_add(proc_t *p); //agrega un proceso a cola, devuelve 0 si ok
uint64_t schedule(uint64_t current_rsp);

#endif // SCHEDULER_H