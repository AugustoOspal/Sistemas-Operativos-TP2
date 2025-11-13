#ifndef _STRINGLIB_H_
#define _STRINGLIB_H_

#include <stdint.h>

uint8_t strlen(const char *str);
uint8_t strcmp(const char *s1, const char *s2);

/*
 *	@brief Convierte el entero en formato string que le pasemos a entero
 *	@param str String con el número a convertir
 *	@return Número del string convertido
 */
int atoi(const char *str);

/*
 *	@brief Convierte el numero que le pasemos en la base que le pasemos a string
 *	@param num Numero a convertir
 *	@param str Puntero al srtring donde vamos a guardar el string del numero
 *	@param base Base del numero que le pasamos
 */
void itoa(int num, char *str, int base);

#endif