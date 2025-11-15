// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "process.h"
#include "../sched/include/scheduler.h"
#include "pmem.h"
#include "lib.h"

extern void *initializeProcess(startWrapperPtr startWrapper, void *stack, mainFuncPtr main, int argc, char *argv[]);

static int argsLen(char **args)
{
	int i = 0;
	if (args == NULL)
		return 0;
	while (args[i] != NULL)
		i++;
	return i;
}

static char **dupArgs(char **args)
{
	if (args == NULL || args[0] == NULL)
	{
		char **newArgs = mem_alloc(sizeof(char *));
		if (newArgs == NULL)
			return NULL;
		newArgs[0] = NULL;
		return newArgs;
	}

	int count = argsLen(args);
	char **newArgs = mem_alloc((count + 1) * sizeof(char *));
	if (newArgs == NULL)
		return NULL;

	for (int i = 0; i < count; i++)
	{
		int len = strlenght(args[i]) + 1;
		newArgs[i] = mem_alloc(len);
		if (newArgs[i] == NULL)
		{
			for (int j = 0; j < i; j++)
				mem_free(newArgs[j]);
			mem_free(newArgs);
			return NULL;
		}
		memcpy(newArgs[i], args[i], len);
	}
	newArgs[count] = NULL;
	return newArgs;
}

void startWrapper(const mainFuncPtr main, const int argc, char *argv[])
{
	main(argc, argv);

	// TODO: Mejorar esto, tendriamos que hacer un exit() accesible desde userland
	killProcess(getPid());
}

uint64_t createProcess(const char *name, const mainFuncPtr main, const int argc, char *argv[], const int16_t fds[],
					   bool foreground)
{
	// TODO: Validar malloc
	void *stackStart = mem_alloc(STACK_SIZE);
	void *stackEnd = stackStart + STACK_SIZE;

	// Copiar argumentos al heap
	char **newArgv = dupArgs(argv);
	if (newArgv == NULL && argv != NULL)
	{
		mem_free(stackStart);
		return 0;
	}

	void *processStackPointer = initializeProcess(startWrapper, stackEnd, main, argc, newArgv);
	const uint64_t newProc = addProcess(processStackPointer, fds);

	// Si devuelve 0 (idle) lo devuelve en caso de error
	if (newProc)
		addProcessInfo(newProc, name, stackStart, newArgv, foreground);

	return newProc;
}

int killProcess(const uint64_t pid)
{
	return terminateProcess(pid);
}