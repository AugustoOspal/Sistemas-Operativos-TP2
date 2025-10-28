#include "include/process.h"
#include "../../mem/include/pmem.h"
#include "lib.h"

#define KERNEL_STACK_SIZE      (16 * 1024)
#define KERNEL_CODE_SELECTOR   0x08
#define KERNEL_DATA_SELECTOR   0x10
#define INITIAL_RFLAGS         0x202

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

//Inicializa las estructuras globales del gestor de procesos, creo que no es neceario porque se inicializan así
void proc_init(void) {
    proc_list_head = NULL;
    proc_list_tail = NULL;
    current_proc = NULL;
    next_pid = 1;
}

// Crea un nuevo proceso, entry_point es la direccion donde comienza la ejecucion del proceso
proc_t *proc_create(void (*entry_point)(void *), void *args) {
    proc_t *proc_mem = (proc_t *)mem_alloc(sizeof(proc_t));
    if (proc_mem == NULL) {
        return NULL;
    }

    void *stack = mem_alloc(KERNEL_STACK_SIZE);
    if (stack == NULL) {
        mem_free(proc_mem);
        return NULL;
    }

    TrapFrame *tf = (TrapFrame *)mem_alloc(sizeof(TrapFrame));
    if (tf == NULL) {
        mem_free(stack);
        mem_free(proc_mem);
        return NULL;
    }

    //Inicializar el TrapFrame
    memset(tf, 0, sizeof(TrapFrame));

    uint8_t *stack_top = (uint8_t *)stack + KERNEL_STACK_SIZE;

    tf->rip = (uint64_t)entry_point;
    tf->rdi = (uint64_t)args;
    tf->rsp = (uint64_t)stack_top;
    tf->cs = KERNEL_CODE_SELECTOR;
    tf->ss = KERNEL_DATA_SELECTOR;
    tf->rflags = INITIAL_RFLAGS;

    proc_mem->name = "unnamed"; //Todo: que reciba un nombre por parametro
    proc_mem->pid = next_pid++;
    proc_mem->state = PROCESS_READY;
    proc_mem->kernel_stack_base = stack;
    proc_mem->kernel_stack_size = KERNEL_STACK_SIZE;
    proc_mem->stack_pointer = stack_top;
    proc_mem->trap_frame = tf;
    proc_mem->page_directory = NULL;
    proc_mem->next = NULL;

    proc_append(proc_mem);

    if (current_proc == NULL) {
        proc_set_current(proc_mem);
    }

    //Devuelvo el proceso creado
    return proc_mem;
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
