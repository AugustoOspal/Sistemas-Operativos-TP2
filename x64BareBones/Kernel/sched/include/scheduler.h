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

//despues
void scheduler_yield(void);
void scheduler_block(proc_t *p); // ready/running -> blocked
void scheduler_unblock(proc_t *p); // blocked -> ready
void scheduler_set_priority(proc_t *p, int prio);
void scheduler_remove(proc_t *p); // ready/blocked -> none
void scheduler_kill(proc_t *p);   

#endif // SCHEDULER_H