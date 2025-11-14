#ifndef SISTEMAS_OPERATIVOS_TP2_OFFICIAL_UTILS_H
#define SISTEMAS_OPERATIVOS_TP2_OFFICIAL_UTILS_H

#include <stdint.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2

uint32_t GetUint();
uint32_t GetUniform(uint32_t max);
uint8_t memcheck(void *start, uint8_t value, uint32_t size);
int64_t satoi(char *str);
void bussy_wait(uint64_t n);
void endless_loop();
void endless_loop_print(uint64_t wait);

#endif
