#ifndef PIPES_H_
#define PIPES_H_

#include "../../semaphore/include/semaphore.h"
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>

#define MAX_PIPES 8
#define PIPE_BUFFER 256
#define SEM_NAME_SIZE 32
#define BASIC_FDS 3 // 0,1,2 reservados para stdin, stdout, stderr
#define PIPE_FDS_AMOUNT 2

/**
 *	@brief Crea un pipe y devuelve un array con sus dos file descriptors
 *	@param fds Array donde se van a almacenar los file descriptors del pipe.
 *	fd[0] es el fd de lectura y fd[1] es el fd de escritura
 *	@return Devuelve el pipeID si fue exitoso y -1 si hubo un error
 */
int16_t pipe_open(int16_t fds[2]);

/**
 *	@brief Escribe datos a un pipe
 *	@param pipe_id ID del pipe al que le vamos a escribir
 *	@param buffer Buffer de donde vamos a sacar los datos que le vamos a poner al pipe
 *	@param count Cantidad de caracteres a copiar del buffer al pipe
 *	@return Cantidad de bytes escritos
 */
int16_t pipe_write(const int16_t fd, const char *buffer, const int count);
/**
 *	@brief Lee datos de un pipe
 *	@param pipe_id ID del pipe del que vamos a leer
 *	@param buffer Buffer de donde vamos a guardar los datos que leemos del pipe
 *	@param count Cantidad de caracteres que vamos a leer del pipe y guardar en el buffer
 *	@return Cantidad de bytes leidos
 */
int16_t pipe_read(const int16_t fd, char *buffer, const int count);

/**
 * @brief Libera los recursos asociados con el pipe
 * @param pipe_id
 * @return En caso de éxito 0 y en otro caso -1
 */
int16_t pipe_close(int pipe_id);

/**
 * @brief Encuentra el índice (ID) de un pipe dado un file descriptor
 * @param fd File descriptor del pipe
 * @return ID del pipe si es válido, -1 si no existe
 */
int16_t get_pipe_idx(int16_t fd);

#endif