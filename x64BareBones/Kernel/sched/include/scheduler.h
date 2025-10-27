#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "process.h"

void scheduler_init(void);
void scheduler_add(proc_t *p);
void scheduler_on_tick(TrapFrame *tf);
void schedule(TrapFrame *tf);
proc_t* scheduler_current(void);

#endif // SCHEDULER_H