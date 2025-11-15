#ifndef SHELL_H_
#define SHELL_H_

#include "mem.h"
#include "pongisLib.h"
#include "../../ReadersWriters/lib/readersWriters.h"
#include "stringLib.h"
#include "test_processes.h" // TODO: Despues sacar esto
#include "timeLib.h"
#include "../../Tests/include/tests.h"
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
	int (*function)(int argc, char *argv[]);
} command_entry;

typedef int (*mainFuncPtr)(int argc, char *argv[]);

void startShell();
void readInput(char *buffer);
int help(int argc, char *argv[]);
int printDateTime(int argc, char *argv[]);
void notACommand(const char *input);
int getRegisters(int argc, char *argv[]);
int exitShell(int argc, char *argv[]);
int clear_screen(int argc, char *argv[]);
int startPongis(int argc, char *argv[]);
int zoom_in(int argc, char *argv[]);
int zoom_out(int argc, char *argv[]);
int busy_wait(int argc, char *argv[]);
int busy_wait_kernel(int argc, char *argv[]);
int runPs(int argc, char *argv[]);
int runCat(int argc, char *argv[]);
int runMem(int argc, char *argv[]);
int runLoop(int argc, char *argv[]);
int runWc(int argc, char *argv[]);
int runFilter(int argc, char *argv[]);
int runKill(int argc, char *argv[]);
int runNice(int argc, char *argv[]);
int runBlock(int argc, char *argv[]);
int runMvar(int argc, char *argv[]);
int runTestMM(int argc, char *argv[]);
int runTestPrio(int argc, char *argv[]);
int runTestProcesses(int argc, char *argv[]);
int runTestSync(int argc, char *argv[]);



#endif
