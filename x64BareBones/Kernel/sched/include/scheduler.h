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

typedef struct ProcessCDT *ProcessADT;

/*
 *  @brief Inicializa el scheduler
 */
void initializeScheduler();

/*
 *  @brief Devuelve el Stack Pointer del proximo proceso a correr y
 *  guarda el Stack Pointer que estaba corriendo
 *  @param stackPointer Stack Pointer del proceso que esta corriendo actualmente
 *  @return Stack Pointer del proceso a correr proximo
 */
void *schedule(void *stackPointer);

/*
 *  @brief Agrega el proceso en el scheduler
 *  @param stackPointer Stack Pointer del proceso a agregar al scheduler
 *  @return PID del proceso agregado al scheduler
 */
uint64_t addProcess(void *stackPointer);

/*
 *  @param pid del proceso a agregar informacion
 *  @param name nombre a asignarle al proceso
 *  @param basePointer a asignarle al proceso
 */
void addProcessInfo(uint64_t pid, const char *name, void *basePointer);

/*
 *  @param pid del proceso a cambiar la prioridad
 *  @param newPriority nueva prioridad a asignarle al proceso
 */
void changeProcessPriority(uint64_t pid, uint8_t newPriority);

/*
 *  @param pid del proceso a remover del scheduler
 */
void terminateProcess(uint64_t pid);

/*
 *  @param pid del proceso a bloquear
 */
void blockProcess(uint64_t pid);

/*
 *  @param pid del proceso a desbloquear
 */
void unblockProcess(uint64_t pid);

/*
 *  @brief Fuerza un timmer interrupt y decrece el quantum del proceso actual
 */
void yield();

/*
 *  @return PID del proceso corriendo actualmente
 */
uint64_t getPid();

/*
 *  @brief Escribe la información del proceso en formato CSV en el buffer
 *  @param pid PID del proceso
 *  @param buffer Buffer donde escribir la información
 *  @param bufferSize Tamaño del buffer
 *  @return Cantidad de caracteres escritos (sin contar el '\0'), o -1 si el proceso no existe
 */
int getProcessInfo(uint64_t pid, char *buffer, uint64_t bufferSize);

/*
 *  @brief Escribe la información de todos los procesos en formato CSV en el buffer
 *  @param buffer Buffer donde escribir la información (incluye header CSV)
 *  @param bufferSize Tamaño del buffer
 *  @return Cantidad de procesos escritos (sin contar el header)
 */
uint64_t getAllProcessesInfo(char *buffer, uint64_t bufferSize);

// resignTimeWindow(); // usa yield

#endif
