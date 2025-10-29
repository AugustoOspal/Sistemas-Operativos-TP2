#include "scheduler.h"
#include "process.h"
#include <stdint.h>

#define QUANTUM 5 // ticks por proceso

extern proc_t *idle_init();
extern void context_switch(uint64_t *prev_sp, uint64_t next_sp);

static proc_t *ready_head = 0;
static proc_t *ready_tail = 0;
static proc_t *current = 0;
static proc_t *idle = 0;

static inline void enqueue(proc_t *p) { // agrega al final
	p->ready_next = 0;
	if (!ready_tail) { // cola vacia
		ready_head = p;
		ready_tail = p;
	}
	else {
		ready_tail->ready_next = p;
		ready_tail = p;
	}
}

static inline proc_t *dequeue() {
	proc_t *p = ready_head;
	if (!p)
		return 0;
	ready_head = p->ready_next;
	if (!ready_head)
		ready_tail = 0;
	p->ready_next = 0;
	return p;
}

static inline proc_t *pick_next() {
	proc_t *p = dequeue();
	return p ? p : idle; // si no hay nadie listo corre idle
}

static inline void switch_to(proc_t *next) {
	if (next == current) {
		return;
	}

	proc_t *prev = current;
	current = next;

	// reencolar al anterior si no es idle y estaba corriendo
	if (prev && prev != idle && prev->state == PROCESS_RUNNING) {
		prev->state = PROCESS_READY;
		enqueue(prev);
	}

	// preparo el nuevo
	current->state = PROCESS_RUNNING;
	current->time_slice_left = QUANTUM;
	proc_set_current(current);
	context_switch((uint64_t *) &prev->stack_pointer, (uint64_t) current->stack_pointer);
}

void scheduler_init(void) {
	ready_head = 0;
	ready_tail = 0;
	idle = idle_init();
	current = idle;
	current->state = PROCESS_RUNNING;
	current->time_slice_left = QUANTUM;
	proc_set_current(current);
}

void scheduler_add(proc_t *p) {
	p->state = PROCESS_READY;
	p->time_slice_left = QUANTUM;
	enqueue(p);
}

proc_t *scheduler_current(void) {
	return current;
}

void schedule(TrapFrame *tf) {
	if (tf) {
		current->stack_pointer = (void *) tf;
	}
	proc_t *next = pick_next();
	if (next != current)
		switch_to(next);
}

void scheduler_on_tick(TrapFrame *tf) {
	current->stack_pointer = (void *) tf;

	if (current == idle) {
		proc_t *n = dequeue();
		if (n) {
			switch_to(n);
		}
		return;
	}
	if (--current->time_slice_left > 0) {
		return;
	}
	else {
		// quantum terminó -> cambio a otro
		switch_to(pick_next());
	}
}
