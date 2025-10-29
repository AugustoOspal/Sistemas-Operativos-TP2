#include "include/process.h"
#include "../../mem/include/pmem.h"
#include "lib.h"
#include "scheduler.h"

#define KERNEL_STACK_SIZE (16 * 1024)
#define KERNEL_CODE_SELECTOR 0x08
#define KERNEL_DATA_SELECTOR 0x10
#define INITIAL_RFLAGS 0x202

static proc_t *proc_list_head = NULL;
static proc_t *proc_list_tail = NULL;
static proc_t *current_proc = NULL;
static pid_t next_pid = 1;

static void proc_append(proc_t *p) {
	if (proc_list_head == NULL) {
		proc_list_head = p;
		proc_list_tail = p;
		return;
	}

	proc_list_tail->next = p;
	proc_list_tail = p;
}

// Inicializa las estructuras globales del gestor de procesos, creo que no es neceario porque se inicializan así
void proc_init(void) {
	proc_list_head = NULL;
	proc_list_tail = NULL;
	current_proc = NULL;
	next_pid = 1;
}

proc_t *proc_create(void (*entry_point)(void *), void *args) {
	proc_t *p = mem_alloc(sizeof(proc_t));
	if (!p)
		return NULL;

	void *kstack = mem_alloc(KERNEL_STACK_SIZE);
	if (!kstack) {
		mem_free(p);
		return NULL;
	}

	uint8_t *stack_top = (uint8_t *) kstack + KERNEL_STACK_SIZE;

	// Construir TrapFrame en la pila del proceso
	TrapFrame *tf = (TrapFrame *) (stack_top - sizeof(TrapFrame));
	memset(tf, 0, sizeof(*tf));

	tf->rip = (uint64_t) entry_point;
	tf->rdi = (uint64_t) args;
	tf->cs = KERNEL_CODE_SELECTOR;
	tf->ss = KERNEL_DATA_SELECTOR;
	tf->rflags = INITIAL_RFLAGS | (1 << 9); // IF=1 por las dudas
	tf->rsp = (uint64_t) stack_top;			// stack “en ejecución” del proceso

	// Inicializar proc_t
	p->name = "unnamed";
	p->pid = next_pid++;
	p->state = PROCESS_READY;
	p->kernel_stack_base = kstack;
	p->kernel_stack_size = KERNEL_STACK_SIZE;
	p->stack_pointer = (void *) tf; // *** clave: SP apunta al TrapFrame
	p->trap_frame = tf;
	p->page_directory = NULL;
	p->next = NULL; // ojo: no la reuses para 2 listas

	proc_append(p);	  // lista global (ok)
	scheduler_add(p); // *** encolarlo para RUN

	return p;
}

void proc_exit(void) {
	if (current_proc == NULL) {
		return;
	}

	current_proc->state = PROCESS_TERMINATED;

	// TODO: Reclamar el kernel stack y el trap frame una vez que el scheduler cambie de proceso.
}

pid_t proc_get_current_pid(void) {
	return current_proc != NULL ? current_proc->pid : -1;
}

proc_t *proc_get_current(void) {
	return current_proc;
}

void proc_set_current(proc_t *p) {
	current_proc = p;
	if (current_proc != NULL) {
		current_proc->state = PROCESS_RUNNING;
	}
}
