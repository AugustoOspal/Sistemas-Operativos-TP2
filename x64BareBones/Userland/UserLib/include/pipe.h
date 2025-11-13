#ifndef SISTEMAS_OPERATIVOS_TP2_PIPE_H
#define SISTEMAS_OPERATIVOS_TP2_PIPE_H

#include <stdint.h>

/**
 *	@brief Crea un pipe y devuelve un array con sus dos file descriptors
 *	@param fds Array donde se van a almacenar los file descriptors del pipe.
 *	fd[0] es el fd de lectura y fd[1] es el fd de escritura
 *	@return Devuelve el pipeID si fue exitoso y -1 si hubo un error
 */
int16_t pipeOpen(int16_t fds[2]);

/**
 * @brief Libera los recursos asociados con el pipe
 * @param pipe_id
 * @return En caso de éxito 0 y en otro caso -1
 */
int16_t pipeClose(int pipe_id);

#endif // SISTEMAS_OPERATIVOS_TP2_PIPE_H
