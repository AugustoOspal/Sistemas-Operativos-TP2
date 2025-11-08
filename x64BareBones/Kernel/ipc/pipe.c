#include "pipe.h"
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>


pipe_t * pipes[MAX_PIPES] = {NULL};

int pipe_open(){

    for(int i = 0; i < MAX_PIPES; i++){
        if(pipes[i] == NULL){ // encuentra un espacio para crear un pipe

			pipes[i] = mem_alloc(sizeof(pipe_t));

            if(pipes[i] == NULL){
                return -1; // error de malloc
            }

            // inicializar pipe
            pipes[i]->readingIdx = 0;
            pipes[i]->toBeRead = 0;
            pipes[i]->writePos = 0;
            pipes[i]->can_read = false;  // al principio no hay nada para leer
            pipes[i]->can_write = true;  // al principio está vacío, se puede escribir

            return i; // retorno el id del pipe creado
        }
    }

    return -1; // no hay espacio para mas pipes
}


//Un proceso llama a pipe_write para escribir n bytes en el buffer del pipe
int pipe_write(int pipe_id, char *buffer, int count) {

    if(pipe_id < 0 || pipe_id >= MAX_PIPES || pipes[pipe_id] == NULL)
        return -1; // pipe_id inválido

    pipe_t *pipe = pipes[pipe_id];

    int written = 0;

    for (; written < count && pipe->toBeRead < MAX_BUFFER ; written++) {
        //semWait
        pipe->buffer[pipe->writePos] = buffer[written];
        pipe->writePos = (pipe->writePos + 1) % MAX_BUFFER; // Buffer circular
        pipe->toBeRead++;
        //semPost
    }

    //flags
    if (pipe->toBeRead > 0)
        pipe->can_read = true;

    pipe->can_write = (pipe->toBeRead < MAX_BUFFER);

    return written;    //retorno la cantidad de bytes escritos

}

pipe_read(int pipe_id, char* buffer, int count){
    if(pipe_id < 0 || pipe_id >= MAX_PIPES || pipes[pipe_id] == NULL)
        return -1; // pipe_id inválido

    pipe_t *pipe = pipes[pipe_id];

    int read = 0;

    for (; read < count && pipe->toBeRead > 0 ; read++) {
        //semWait
        buffer[read] = pipe->buffer[pipe->readingIdx];
        pipe->readingIdx = (pipe->readingIdx + 1) % MAX_BUFFER; // Buffer circular
        pipe->toBeRead--;
        //semPost
    }

    // flags
    pipe->can_read = (pipe->toBeRead > 0);
    pipe->can_write = (pipe->toBeRead < MAX_BUFFER);

    return read;    //retorno la cantidad de bytes leidos
}

//liberar los recursos del pipe
int pipe_close(int pipe_id) {
    // Verificar si el pipe_id es válido
    if (pipe_id < 0 || pipe_id >= MAX_PIPES || pipes[pipe_id] == NULL)
        return -1;


    // Liberar memoria del pipe
    mem_free(pipes[pipe_id]);
    pipes[pipe_id] = NULL;


    return 0; // Cierre exitoso
}