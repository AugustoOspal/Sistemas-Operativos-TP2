#ifndef _EXCEPTIONS_H_
#define _EXCEPTIONS_H_

#include <color.h>
#include <fonts.h>
#include <lib.h>
#include <syscalls.h>
#include <videoDriver.h>

#define ZERO_EXCEPTION_ID 0
#define INVALID_OPERATION_CODE_ID 6

void exceptionDispatcher(Registers_t *regs, int exception);

#endif