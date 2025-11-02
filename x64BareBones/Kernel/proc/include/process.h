#ifndef PROCESS_H
#define PROCESS_H

#include "memory.h"
#include <stdint.h>

#define STACK_SIZE 4096

typedef int (*mainFuncPtr)(int argc, char* argv[]);
typedef void (*startWrapperPtr)(mainFuncPtr main, int argc, char* argv[]);

void createProcess(mainFuncPtr main, int argc, char *argv[]);
void deleteProcess(uint64_t pid);
extern void *initializeProcess(startWrapperPtr startWrapper, void *stack, mainFuncPtr main, int argc, char *argv[]);

#endif

