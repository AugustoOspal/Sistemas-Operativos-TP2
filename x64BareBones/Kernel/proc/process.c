#include "process.h"
#include "pmem.h"
#include <string.h>
#include "cpu_helpers.h"

#define KERNEL_STACK_SIZE (16 * 4096)
#define INITIAL_RFLAGS 0x202

static int next_pid = 1;

proc_t *proc_create(void (*entry_point)(void *), void *arg){
    /* reservar pila kernel */
    uint8_t *stack = mem_alloc(KERNEL_STACK_SIZE);
    if(stack == NULL){
        return NULL;
    }

    /* reservar estructura proc */
    proc_t *p = mem_alloc(sizeof(proc_t));
    if(p == NULL){
        mem_free(stack);
        return NULL;
    }

    /* colocar TrapFrame al tope de la pila (stack crece hacia abajo) */
    uint8_t *stack_top = stack + KERNEL_STACK_SIZE;
    TrapFrame *tf = (TrapFrame *)(stack_top - sizeof(TrapFrame));
    memset(tf, 0, sizeof(TrapFrame));

    /* inicializar campos del trapframe */
    tf->rip    = (uint64_t)entry_point;       /* entry */
    tf->rdi    = (uint64_t)arg;               /* primer arg (ABI SysV) */
    tf->cs     = (uint64_t)read_cs();         /* suponemos ring 0 por ahora */
    tf->rflags = (uint64_t)INITIAL_RFLAGS;    /* IF=1 entre otros */
    /* establecer un rsp "usuario" simulado (no usado en ring0) */
    tf->rsp    = (uint64_t)(stack_top - 0x100);
    tf->ss     = 0;                           /* si solo kernel, ok=0 */

    /* inicializar proc */
    memset(p, 0, sizeof(proc_t));
    p->kernel_stack_base = stack;
    //p->kernel_stack_top  = stack_top;
    p->kernel_stack_size = KERNEL_STACK_SIZE;
    p->entry_point       = entry_point;
    p->state             = PROCESS_READY;
    p->pid               = next_pid++;
    p->saved_rsp         = (uint64_t)tf;     /* rsp apuntando al inicio del TrapFrame */

    return p;
}

void proc_destroy(proc_t *p){
    if(p == NULL) return;
    if(p->kernel_stack_base) mem_free(p->kernel_stack_base);
    mem_free(p);
}

void proc_set_state(proc_t *p, proc_state_t new_state){
    if(p == NULL) return;
    p->state = new_state;
}

/* Cede CPU voluntariamente */
void proc_yield(void){
    //schedule();
}
