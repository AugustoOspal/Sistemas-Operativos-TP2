#ifndef PROCESS_H
#define PROCESS_H

#include "memory.h"
#include <stdint.h>

#define STACK_SIZE 4096

typedef int (*mainFuncPtr)(int argc, char *argv[]);
typedef void (*startWrapperPtr)(mainFuncPtr main, int argc, char *argv[]);

/*
 *  @brief Crea un proceso añadiéndolo al scheduler
 *  @param name Nombre del proceso
 *  @param main Puntero a la función main
 *  @param argc Cantidad de argumentos
 *  @param argv Arreglo de strings con los argumentos
 *  @param fds Array con los file descriptors del proceso
 *  @return El PID del proceso creado
 */
//Todo: que no quede fds[magic number], es el mismo que el de scheduler.h --> FD_AMOUNT
uint64_t createProcess(const char *name, const mainFuncPtr main, const int argc, char *argv[], int fds[3]);

/*
 *  @brief Elimina un proceso del scheduler y libera su memoria
 *  @param pid El PID del proceso a eliminar
 */
void killProcess(uint64_t pid);

#endif
