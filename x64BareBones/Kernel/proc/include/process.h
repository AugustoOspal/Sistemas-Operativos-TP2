#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct TrapFrame {
    /* Registros salvados por software (orden fijo) */
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rsi, rdi, rbp, rdx, rcx, rbx, rax;
    /* Lo que iretq espera (orden: RIP, CS, RFLAGS, RSP, SS) */
    uint64_t rip, cs, rflags;
    uint64_t rsp, ss;
} TrapFrame;


typedef enum {
    PROCESS_READY,
    PROCESS_RUNNING,
    PROCESS_ZOMBIE,
} proc_state_t;

typedef struct proc {
    int pid;
    proc_state_t state;
    uint8_t *kernel_stack_base; //puntero a la base del bloque de memoria que reservamos para su pila de kernel
    size_t kernel_stack_size; //tamaño de la pila de kernel para validaciones y liberaciones
    void (*entry_point)(void *); //punto de entrada del proceso
    uint64_t     saved_rsp; //el rsp apunta al trapframe y con eso recupero contexto
} proc_t;

proc_t *proc_create(void (*entry_point)(void *), void *arg); //el primer parametro es un puntero a funcion que recibe void * (lo que sea) y no devuelve nada
void proc_destroy(proc_t *p);
void proc_set_state(proc_t *p, proc_state_t new_state);
void proc_yield(void);

#endif // PROCESS_H