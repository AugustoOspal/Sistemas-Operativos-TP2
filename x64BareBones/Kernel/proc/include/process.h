#ifndef PROCESS_H
#define PROCESS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct TrapFrame {
	// guardados por pushState mismo orden
	uint64_t rax;
	uint64_t rbx;
	uint64_t rcx;
	uint64_t rdx;
	uint64_t rbp;
	uint64_t rdi;
	uint64_t rsi;
	uint64_t r8;
	uint64_t r9;
	uint64_t r10;
	uint64_t r11;
	uint64_t r12;
	uint64_t r13;
	uint64_t r14;
	uint64_t r15;
	// iret frame que ya estaba en la pila abajo de los pushes
	uint64_t rip;
	uint64_t cs;
	uint64_t rflags;
	uint64_t rsp;
	uint64_t ss;
} TrapFrame;

typedef int pid_t;

typedef enum { PROCESS_READY, PROCESS_RUNNING, PROCESS_BLOCKED, PROCESS_TERMINATED, PROCESS_ZOMBIE } proc_state_t;

// Estructura que representa un proceso en el sistema
typedef struct proc {
	pid_t pid;
	const char *name;
	proc_state_t state;
	int time_slice_left;

	void *kernel_stack_base;
	size_t kernel_stack_size;
	void *stack_pointer;
	TrapFrame *trap_frame;

	void *page_directory;

	struct proc *next;		 // lista global
	struct proc *ready_next; // ready queue

} proc_t;

void proc_init(void);
proc_t *proc_create(void (*entry_point)(void *), void *args);
void proc_exit(void);
pid_t proc_get_current_pid(void);
proc_t *proc_get_current(void);
void proc_set_current(proc_t *p);

#endif // PROCESS_H
