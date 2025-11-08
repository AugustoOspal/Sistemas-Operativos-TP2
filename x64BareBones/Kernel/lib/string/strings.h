#ifndef STRINGS_H
#define STRINGS_H

#include "pmem.h"
#include <stdarg.h>
#include <stddef.h>

char *strCpy(char *dest, const char *src);
size_t strLen(const char *str);
char *strDup(const char *src);
int ksprintf(char *str, const char *format, ...);

#endif
