#ifndef _PIPES_H_
#define _PIPES_H_

#include <stdint.h>
#include <stdbool.h>

#define MAX_PIPES 20
#define MAX_BUFFER 256


typedef struct{
    char buffer[MAX_BUFFER]; // guarda los datos escritos en el pipe
    int readingIdx; // indice de lectura 
    int toBeRead; // cantidad de datos pendientes de leer
    int writePos; // indice de escritura

    bool can_read;
    bool can_write;
} pipe_t;

int pipe_open();
pipe_read(int pipe_id, char* buffer, int count);
void pipe_close(int pipeId);

#endif