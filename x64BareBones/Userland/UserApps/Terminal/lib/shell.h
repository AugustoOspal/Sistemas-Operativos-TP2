#ifndef _SHELL_H_
#define _SHELL_H_

#include "pongisLib.h"
#include "stringLib.h"
#include "test_processes.h" // TODO: Despues sacar esto
#include "timeLib.h"
#include "usrio.h"
#include "videoLib.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2

typedef enum
{
	CMD_BUILTIN,
	CMD_PROC
} command_type;

typedef struct
{
	char *name;
	command_type type;
	void (*function)(); // puntero a la funcion, solo si es  built in
} command_entry;

typedef enum
{
	HELP = 0,
	EXC_1,
	EXC_2,
	PONGISGOLF,
	ZOOM_IN,
	ZOOM_OUT,
	CLEAR,
	DATE,
	REGISTERS,
	BUSY_WAIT,
	BUSY_WAIT_KERNEL,
	EXIT
} command_id;

void startShell();
void readInput();
command_entry *findCommand(char *input);
command_id processInput(char *input);
void help();
void printDateTime();
void notACommand();
void getRegisters();
void exitShell();
void clear_screen();
void startPongis();
void zoom_in();
void zoom_out();
void exception_1();
void exception_2();
void busy_wait();
void busy_wait_kernel();
void runPs();
void runCat();

#endif