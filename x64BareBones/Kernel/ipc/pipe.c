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

            // inicializar semaforos
            char semName[SEM_NAME_SIZE];

            // Semáforo de escritura 
            itoa(i, semName);                // convierte i a string
            strcat(semName, "_write");
            pipes[i]->writeSem = semOpen(semName, MAX_BUFFER);
            if (pipes[i]->writeSem == NULL) {
                mem_free(pipes[i]);
                pipes[i] = NULL;
                return -1;
            }

            // Semáforo de lectura 
            itoa(i, semName);                // convierte i a string
            strcat(semName, "_read");
            pipes[i]->readSem = semOpen(semName, 0);
            if (pipes[i]->readSem == NULL) {
                sem_unlink(semName);  // liberar write_sem si falla
                semClose(pipes[i]->writeSem);
                mem_free(pipes[i]);
                pipes[i] = NULL;
                return -1;
            }

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
        // Espera a que haya espacio disponible
        semWait(pipe->writeSem);

        // Escribir en el buffer del pipe
        pipe->buffer[pipe->writePos] = buffer[written];
        pipe->writePos = (pipe->writePos + 1) % MAX_BUFFER; // Buffer circular
        pipe->toBeRead++;

        // Avisa que hay datos para leer
        semPost(pipe->readSem);
    }


    return written;    //retorno la cantidad de bytes escritos

}

int pipe_read(int pipe_id, char* buffer, int count){
    if(pipe_id < 0 || pipe_id >= MAX_PIPES || pipes[pipe_id] == NULL)
        return -1; // pipe_id inválido

    pipe_t *pipe = pipes[pipe_id];

    int read = 0;

    for (; read < count && pipe->toBeRead > 0 ; read++) {
        // Esperar a que haya algo para leer
        semWait(pipe->readSem);

        // Leer un byte del buffer circular        
        buffer[read] = pipe->buffer[pipe->readingIdx];
        pipe->readingIdx = (pipe->readingIdx + 1) % MAX_BUFFER; // Buffer circular
        pipe->toBeRead--;

        // Señalar que hay espacio disponible para escribir
        semPost(pipe->writeSem);    
    }


    return read;    //retorno la cantidad de bytes leidos
}

//liberar los recursos del pipe
int pipe_close(int pipe_id) {
    // Verificar si el pipe_id es válido
    if (pipe_id < 0 || pipe_id >= MAX_PIPES || pipes[pipe_id] == NULL)
        return -1;
    pipe_t *pipe = pipes[pipe_id];

    // Cerrar semáforos
    semClose(pipe->readSem);
    semClose(pipe->writeSem);   
    
    // Eliminar semáforos
    sem_unlink(pipe->readSem->name);
    sem_unlink(pipe->writeSem->name);

    // Liberar memoria del pipe
    mem_free(pipe);
    pipe = NULL;

    return 0; // Cierre exitoso
}