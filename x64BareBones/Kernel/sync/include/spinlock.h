/*
 *  Está hecho como en la página 271 del libro de Abraham-Silberschatz
 *  Operating systems 10th 2018
 */

#ifndef SISTEMAS_OPERATIVOS_TP2_SPINLOCK_H
#define SISTEMAS_OPERATIVOS_TP2_SPINLOCK_H

#include <stdint.h>

/*
 *  @brief Hace busy waiting esperando a que se libere el lock
 *  @param lock del spinlock
 */
void acquire(uint64_t *lock);

/*
 *  @brief Intenta agarrar el lock una vez, pero no sigue intentando si falla
 *  @param lock del spinlock
 *  @return 0 si fue exitoso y agarro el lock, 1 si fallo y no lo pudo agarrar
 *  @warning Ver bien los valores que devuelve, están invertidos
 */
int try_acquire(uint64_t *lock);

void release(uint64_t *lock);

#endif // SISTEMAS_OPERATIVOS_TP2_SPINLOCK_H