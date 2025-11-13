#ifndef SISTEMAS_OPERATIVOS_TP2_PROCESSES_H
#define SISTEMAS_OPERATIVOS_TP2_PROCESSES_H

#include "syscallLib.h"
#include <stdint.h>

/*
 *  @brief Creates a new process
 *  @param name Name of the process
 *  @param main Pointer to the main function of the process
 *  @param argc Number of arguments
 *  @param argv Array of arguments
 *  @param fds Array of file descriptors
 *  @param foreground Whether process runs in foreground (true) or background (false)
 *  @return PID of the created process
 */
uint64_t createProcess(const char *name, int (*main)(int argc, char *argv[]), int argc, char *argv[],
					   int fds[FD_AMOUNT], bool foreground);

/*
 *  @brief Deletes a process
 *  @param pid PID of the process to delete
 */
void deleteProcess(uint64_t pid);

/*
 *  @brief Gets the PID of current running process
 *  @return PID of current running process
 */
uint64_t getPID();

/*
 *  @brief Gets information of all processes in CSV format
 *  @param buffer Buffer to write the information to
 *  @param bufferSize Size of the buffer
 *  @return Number of processes written
 */
uint64_t getProcessesInfo(char *buffer, uint64_t bufferSize);

/*
 *  @brief Changes the priority of a process
 *  @param pid PID of the process
 *  @param newPriority New priority value
 */
void changeProcessPriority(uint64_t pid, uint8_t newPriority);

void blockProcess(uint64_t pid);
void unblockProcess(uint64_t pid);
void yield();

uint64_t waitPid(uint64_t pid);

/*
 *  @brief implementacion de cat
 *  Imprime el stdin tal como lo recibe.
 */
void cat();

void loop();

void wc();

void filter();

void kill(uint64_t pid);

#endif // SISTEMAS_OPERATIVOS_TP2_PROCESSES_H