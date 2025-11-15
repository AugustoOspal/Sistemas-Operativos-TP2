#ifndef SISTEMAS_OPERATIVOS_TP2_OFFICIAL_SYSCALLS_H
#define SISTEMAS_OPERATIVOS_TP2_OFFICIAL_SYSCALLS_H

#include "../../../UserLib/include/semaphores.h"
#include <stdbool.h>
#include <stdint.h>

typedef int (*mainFuncPtr)(int argc, char *argv[]);

int64_t my_getpid();
uint64_t my_create_process(const char *name, mainFuncPtr main, int argc, char *argv[], int16_t fds[3], bool foreground);
int64_t my_nice(uint64_t pid, uint64_t newPrio);
int my_kill(uint64_t pid);
int64_t my_block(uint64_t pid);
int64_t my_unblock(uint64_t pid);
int64_t my_yield();
int64_t my_wait(int64_t pid);

semaphoreP my_sem_open(char *sem_id, int initialValue);
void my_sem_wait(semaphoreP sem);
void my_sem_post(semaphoreP sem);
void my_sem_close(semaphoreP sem);

#endif
