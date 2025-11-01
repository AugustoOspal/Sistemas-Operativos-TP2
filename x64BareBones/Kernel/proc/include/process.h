#ifndef PROCESS_H
#define PROCESS_H

#include "memory.h"

#define STACK_SIZE 4096

typedef int (*mainFuncPtr)(int argc, char* argv[]);
typedef void (*startWrapperPtr)(mainFuncPtr main, int argc, char* argv[]);

void createProcess();
extern void *initializeProcess(startWrapperPtr startWrapper, void *stack, mainFuncPtr main, int argc, char *argv[]);

#endif

