#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "../ADT/DoubleLinkedList/doubleLinkedList.h"
#include "../ADT/Queue/queue.h"
#include "idle.h"
#include "pmem.h"
#include "process.h"
#include <stdbool.h>
#include <stdint.h>

#define QUANTUM 1
#define PRIO 4
#define DEFAULT_PRIORITY 0
#define MAX_CHILDREN 20
#define DEFAULT_FOREGROUND true
#define STDIN 0
#define STDOUT 1
#define STDERR 2

typedef struct ProcessCDT *ProcessADT;

/**
 *  @brief Inicializa el scheduler
 */
void initializeScheduler();

/**
 *  @brief Devuelve el Stack Pointer del proximo proceso a correr y
 *  guarda el Stack Pointer que estaba corriendo
 *  @param stackPointer Stack Pointer del proceso que esta corriendo actualmente
 *  @return Stack Pointer del proceso a correr proximo
 */
void *schedule(void *stackPointer);

/**
 *  @brief Agrega el proceso en el scheduler
 *  @param stackPointer Stack Pointer del proceso a agregar al scheduler
 *  @param fds Array con los file descriptors del proceso
 *  @return PID del proceso agregado al scheduler
 */
uint64_t addProcess(void *stackPointer, const int16_t fds[FD_AMOUNT]);

/**
 *  @param pid del proceso a agregar informacion
 *  @param name nombre a asignarle al proceso
 *  @param basePointer a asignarle al proceso
 *  @param argv puntero al array de argumentos del proceso (heap-allocated)
 *  @param foreground indica si el proceso está en foreground
 */
void addProcessInfo(uint64_t pid, const char *name, void *basePointer, char **argv, bool foreground);

/**
 *  @param pid del proceso a cambiar la prioridad
 *  @param newPriority nueva prioridad a asignarle al proceso
 */
void changeProcessPriority(uint64_t pid, uint8_t newPriority);

/**
 *  @param pid del proceso a remover del scheduler
 */
void terminateProcess(uint64_t pid);

/**
 *  @brief bloquea en ese momento el proceso, llamando al timerInterrupt
 *  @param pid del proceso a bloquear
 */
void blockProcess(uint64_t pid);

/**
 *  @brief Muy parecido a blockProcess, con la diferencia de que
 *  no hace un timerInterrupt cuando lo agrega a la cola
 *  @param pid del proceso a agregar a la cola de bloqueados
 */
void addProcessToBlockQueue(uint64_t pid);

/**
 *  @param pid del proceso a desbloquear
 */
void unblockProcess(uint64_t pid);

/**
 *  @brief Fuerza un timmer interrupt y decrece el quantum del proceso actual
 */
void yield();

/**
 *  @return PID del proceso corriendo actualmente
 */
uint64_t getPid();

/**
 *  @brief Escribe la información del proceso en formato CSV en el buffer
 *  @param pid PID del proceso
 *  @param buffer Buffer donde escribir la información
 *  @param bufferSize Tamaño del buffer
 *  @return Cantidad de caracteres escritos (sin contar el '\0'), o -1 si el proceso no existe
 */
int getProcessInfo(uint64_t pid, char *buffer, uint64_t bufferSize);

/**
 *  @brief Escribe la información de todos los procesos en formato CSV en el buffer
 *  @param buffer Buffer donde escribir la información (incluye header CSV)
 *  @param bufferSize Tamaño del buffer
 *  @return Cantidad de procesos escritos (sin contar el header)
 */
uint64_t getAllProcessesInfo(char *buffer, uint64_t bufferSize);

/**
 *  @brief Espera a que un proceso hijo termine
 *  @param pid PID del proceso hijo a esperar
 *  @param status Puntero donde guardar el estado de salida del proceso hijo (puede ser NULL)
 *  @return PID del proceso hijo que terminó, o -1 en caso de error
 */
uint64_t waitPid(uint64_t pid);

/**
 *	@brief Obtiene el PID del proceso en foreground
 *	@return El PID del proceso que está en foreground, o 0 si ninguno
 */
uint64_t getForegroundPid(void);

/**
 * @brief Obtiene el file descriptor del proceso actual
 * @param fdIndx Índice del file descriptor a obtener
 */
int16_t getProcesFd(const int fdIdx);

/**
 * @brief Obtiene el estado actual de un proceso dado su PID.
 * @param pid PID del proceso a consultar.
 * @return Estado del proceso (READY, RUNNING, BLOCKED, ZOMBIE) o -1 si no existe.
 */
int64_t getProcessStatus(uint64_t pid);

// resignTimeWindow(); // usa yield

#endif
