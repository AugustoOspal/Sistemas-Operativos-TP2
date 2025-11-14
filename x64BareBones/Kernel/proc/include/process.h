#ifndef PROCESS_H
#define PROCESS_H

#include <stdbool.h>
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
 *  @param foreground Indica si el proceso está en foreground (true) o background (false)
 *  @return El PID del proceso creado. Devuelve 0 en caso de error
 */
uint64_t createProcess(const char *name, mainFuncPtr main, int argc, char *argv[], const int16_t fds[],
					   bool foreground);

/**
 *  @brief Elimina un proceso del scheduler y libera su memoria
 *  @param pid El PID del proceso a eliminar
 *  @return Devuelve 0 si tuvo exito o -1 de lo contrario
 */
int killProcess(uint64_t pid);

#endif
