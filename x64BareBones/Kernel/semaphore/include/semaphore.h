/*
 *  Nos inspiramos de la implementacion de:
 *		- Págs 274 y 278 del libro de Abraham-Silberschatz Operating systems 10th 2018
 *		- Págs desde 1133 a 1143 del libro The Linux Programming Interface
 */

#ifndef SISTEMAS_OPERATIVOS_TP2_SEMAPHORE_H
#define SISTEMAS_OPERATIVOS_TP2_SEMAPHORE_H

typedef struct semaphore *semaphoreP;

/**
 *  @brief inicializa la estructura de semaforos.
 */
void semaphoresInit(void);

// TODO: Hacer la documentacion de estos 2
void semPost(semaphoreP sem);
void semWait(semaphoreP sem);

/**
 *  @brief Lo mismo que semWait pero no se bloquea, si no puede acceder al semaforo
 *  retorna error.
 *  @return -1 si no pudo acceder, 0 si tuvo exito
 */
int semTryWait(semaphoreP sem);

/**
 *  @brief Abre un semaforo que cumpla con ese nombre o lo crea en caso de que no exista.
 *  Asocia el semaforo con el proceso que llama a esta función y setea el valor del semaforo en value.
 *  @param name Nombre del semaforo a abrir o crear
 *  @param value Valor inicial del semaforo
 */
semaphoreP semOpen(const char *name, int value);

/**
 *  @brief Cierra la asociacion que había entre el semaforo y el proceso que
 *  llama a esta función y decrece la cantidad de procesos asociados.
 *
 */
void semClose(semaphoreP sem);

/**
 *  @brief Hace inaccesible el semaforo con ese nombre y lo marca para que sea destruido
 *  después de que todos los procesos dejen de usarlo (puede ser inmediatamente si todos los procesos
 *  ya lo cerraron). Si hay procesos esperando el semaforo, pero ningún proceso asociado al semaforo
 *  no se destruye hasta que termine el último proceso en lista de espera. No estaba claro ese
 *  comportamiento en el manual de linux así que lo decidimos así.
 *  @param name nombre del proceso
 *  @return 0 si fue exitoso, -1 si no existe ese semaforo
 *  @warning No se cierra siempre instantaneamente, espera a que lo cierren todos los procesos y que
 *  todos los procesos esperando el semaforo terminen.
 */
int sem_unlink(const char *name);

/**
 *  @param sem Semaforo al que se le quiere ver el valor
 *  @param sval Variable donde se va a almacenar el valor del semaforo o la cantidad de procesos bloqueados
 *  de forma negativa.
 *  @note lo puse así por lo que pone en la pág 1097. Linux devuelve 0, pero como nosotros evidentemente lo
 *  implementamos “ineficientemente” según el libro, aprovechamos y lo dejamos negativo
 */
int semGetValue(semaphoreP sem, int *sval);

/**
 *  @brief Busca un semaforo por su nombre.
 *  @param name Nombre del semaforo a buscar
 *  @return Puntero al semaforo si lo encuentra, NULL si no existe
 */
semaphoreP getSem(const char *name);

#endif // SISTEMAS_OPERATIVOS_TP2_SEMAPHORE_H