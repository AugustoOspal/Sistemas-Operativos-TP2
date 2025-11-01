#include "scheduler.h"
#include <stdint.h>
#include "process.h"
#include "syscalls.h"

#define MAX_PROCS 8

static proc_t *run_queue[MAX_PROCS]; //arreglo circular
static int run_queue_len = 0; //tamaño actual de la cola
static int current_proc = -1; //indice del actual

//recorro de forma circular y devuelvo indice del siguiente o -1 si no hay
static int pick_next(void) {
    if (run_queue_len == 0){ //no hay next
        return -1;
    }
    for (int i = 1; i <= run_queue_len; i++) {
        int idx = (current_proc + i) % run_queue_len; //siguiente en forma circular
        if (run_queue[idx]->state != PROCESS_ZOMBIE){ //al primero que no sea zombie lo elijo
            return idx;
        }
    }
    return -1;
}

void scheduler_init(void) { //ya son inicializados pero en ejecucion se podria reiniciar sistema de planificacion o usos futuros
    run_queue_len = 0;
    current_proc = -1;
    for(int i = 0; i < MAX_PROCS; i++){
        run_queue[i] = 0;
    }
}

int scheduler_add(proc_t *p) {
    if (p == NULL || run_queue_len >= MAX_PROCS){
        return -1;
    }
    p->state = PROCESS_READY;
    run_queue[run_queue_len++] = p;
    return 0;
}

uint64_t schedule(uint64_t current_rsp){
    if (run_queue_len == 0){
        return 0;
    }

    if (current_proc < 0) { //primer tick
        current_proc = 0;
        run_queue[current_proc]->state = PROCESS_RUNNING;
        return run_queue[current_proc]->saved_rsp;
    }

    proc_t * current_process_aux = run_queue[current_proc];
    current_process_aux->saved_rsp = current_rsp;
    if(current_process_aux->state == PROCESS_RUNNING){
        current_process_aux->state = PROCESS_READY;
    }
    int next = pick_next();
    if (next < 0 || next == current_proc){ //no tengo que cambiar de proc
        current_process_aux->state = PROCESS_RUNNING;
        return 0; //se sigue en el mismo
    }

    proc_t * next_process = run_queue[next];
    next_process->state = PROCESS_RUNNING;
    current_proc = next;
    return next_process->saved_rsp;
}