#ifndef SISTEMAS_OPERATIVOS_TP2_PROCESSES_H
#define SISTEMAS_OPERATIVOS_TP2_PROCESSES_H

#include <stdint.h>

/*
 *  @brief Creates a new process
 *  @param name Name of the process
 *  @param main Pointer to the main function of the process
 *  @param argc Number of arguments
 *  @param argv Array of arguments
 *  @return PID of the created process
 */
uint64_t createProcess(const char *name, int (*main)(int argc, char *argv[]), int argc, char *argv[]);

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

#endif // SISTEMAS_OPERATIVOS_TP2_PROCESSES_H