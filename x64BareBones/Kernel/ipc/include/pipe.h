#ifndef _PIPES_H_
#define _PIPES_H_

#include <stdint.h>
#include <stdbool.h>
#include <../../semaphore/include/semaphore.h>

#define MAX_PIPES 20
#define MAX_BUFFER 256
#define SEM_NAME_SIZE 32


typedef struct{
    char buffer[MAX_BUFFER]; // guarda los datos escritos en el pipe
    int readingIdx; // indice de lectura 
    int toBeRead; // cantidad de datos pendientes de leer
    int writePos; // indice de escritura

    semaphoreP writeSem; // representa la cantidad de espacios libres para escribir
    semaphoreP readSem; // representa la cantidad de datos disponibles para leer
} pipe_t;

int pipe_open();
int pipe_write(int pipe_id, char *buffer, int count);
int pipe_read(int pipe_id, char* buffer, int count);
int pipe_close(int pipeId);

#endif