// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../../UserLib/include/mem.h"
#include "../../UserLib/include/processes.h"
#include "../../UserLib/include/semaphores.h"
#include "../../UserLib/include/stringLib.h"
#include "../../UserLib/include/timeLib.h"
#include <stdbool.h>
#include <stdint.h>

static char slot = 0;
static semaphoreP semFull;
static semaphoreP semEmpty;

typedef struct
{
	int id;
	char symbol;
} writer_arg_t;

typedef struct
{
	int id;
} reader_arg_t;

int writerMain(int argc, char **argv)
{
	char *symbol = argv[0];
	while (1)
	{
		semWait(semEmpty);
		slot = symbol[0];
		semPost(semFull);
	}
	return 0;
}

static void readerConsume(char value, char *id)
{
	int index = 0;
	char s[10];
	s[index++] = value;
	s[index++] = '(';
	for (int i = 0; id[i] != 0; i++)
	{
		s[index++] = id[i];
	}
	s[index++] = ')';
	s[index++] = '\n';
	sys_write(1, s, index + 1);
}

int readerMain(int argc, char **argv)
{
	char *id = argv[0];
	while (1)
	{
		sleepMilli(3000);
		semWait(semFull);
		char c = slot;
		slot = 0;
		semPost(semEmpty);
		readerConsume(c, id);
	}
	return 0;
}

int mainMvar(int argc, char **argv)
{
	int writerCount = (argc > 1) ? atoi(argv[1]) : 2;
	int readerCount = (argc > 2) ? atoi(argv[2]) : 2;

	semEmpty = semOpen("mvar_empty", 1);
	semFull = semOpen("mvar_full", 0);

	static const char letters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	for (int i = 0; i < writerCount; i++)
	{
		char *symbol = mem_alloc(2);
		symbol[0] = letters[i % (int) (sizeof(letters) - 1)];
		symbol[1] = '\0';
		char **args = mem_alloc(sizeof(char *) * 2);
		args[0] = symbol;
		args[1] = NULL;
		createProcess("Writer", writerMain, 1, args, NULL, false);
	}

	// char buffers[readerCount][20];  // Buffer no usado
	for (int j = 0; j < readerCount; j++)
	{
		char *id = mem_alloc(6);
		itoa(j, id, 6);
		char **args = mem_alloc(sizeof(char *) * 2);
		args[0] = id;
		args[1] = NULL;
		createProcess("Reader", readerMain, 1, args, NULL, false);
	}

	kill(getPID());
	return 0;
}