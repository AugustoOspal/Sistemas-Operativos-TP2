#include "process.h"
#include "pmem.h"
#include <string.h>

#define KERNEL_STACK_SIZE (16 * 4096)
#define KERNEL_CODE_SELECTOR 0x08
#define INITIAL_RFLAGS 0x202  

static int next_pid = 1;

proc_t *proc_create(void (*entry_point)(void *), void *arg){
    // reservo memoria para pila kernel
    uint8_t *stack = mem_alloc(KERNEL_STACK_SIZE);
    if(stack == NULL){
        return NULL;
    }
    // reservo memoria para estructura proc
    proc_t *p = mem_alloc(sizeof(proc_t));
    if(p == NULL){
        mem_free(stack);
        return NULL;
    }

    // inicializo el trapframe
    TrapFrame *tf = (TrapFrame *)(stack + KERNEL_STACK_SIZE - sizeof(TrapFrame));
    memset(tf, 0, sizeof(TrapFrame));

    tf->rip = (uint64_t)entry_point; // donde empieza el proceso
    tf->rdi = (uint64_t)arg; // primer argumento
    tf->cs  = KERNEL_CODE_SELECTOR;
    tf->rflags = INITIAL_RFLAGS;

    // inicializo estructura proc
    memset(p, 0, sizeof(proc_t));
    p->kernel_stack_base = stack;
    p->kernel_stack_size = KERNEL_STACK_SIZE;
    p->state = PROCESS_READY;
    p->pid = next_pid++;
    p->saved_rsp = (uint64_t)tf;
    return p;
}

void proc_destroy(proc_t *p){
    if(p == NULL){
        return;
    }
    //liberar pila de kernel
    if(p->kernel_stack_base != NULL){
        mem_free(p->kernel_stack_base);
    }
    //liberar estructura proc
    mem_free(p);
}

void proc_set_state(proc_t *p, proc_state_t new_state){
    if(p == NULL){
        return;
    }
    p->state = new_state;
}

void proc_yield(void){
    //implementar
}