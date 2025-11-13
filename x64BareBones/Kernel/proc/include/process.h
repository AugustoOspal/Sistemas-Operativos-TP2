#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>

#define STACK_SIZE 4096
#define FD_AMOUNT 3

typedef int (*mainFuncPtr)(int argc, char *argv[]);
typedef void (*startWrapperPtr)(mainFuncPtr main, int argc, char *argv[]);

/**
 *  @brief Crea un proceso añadiéndolo al scheduler
 *  @param name Nombre del proceso
 *  @param main Puntero a la función main
 *  @param argc Cantidad de argumentos
 *  @param argv Arreglo de strings con los argumentos
 *  @param fds Array con los file descriptors del proceso
 *  @return El PID del proceso creado
 */
uint64_t createProcess(const char *name, mainFuncPtr main, int argc, char *argv[], const int16_t fds[]);

/**
 *  @brief Elimina un proceso del scheduler y libera su memoria
 *  @param pid El PID del proceso a eliminar
 */
void killProcess(uint64_t pid);

#endif
