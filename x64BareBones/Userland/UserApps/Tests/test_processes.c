// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../../UserLib/include/mem.h"
#include "../../UserLib/include/processes.h"
#include "./include/syscall.h"
#include "./include/test_util.h"

#include <stdint.h>
#include <stdio.h>

enum State
{
	RUNNING,
	BLOCKED,
	KILLED
};

typedef struct P_rq
{
	uint64_t pid;
	enum State state;
} p_rq;

static int16_t defaultFds[] = {STDIN, STDOUT, STDERR};

int64_t test_processes(uint64_t argc, char *argv[])
{
	uint8_t rq;
	uint8_t alive = 0;
	uint8_t action;
	uint64_t max_processes;
	char *argvAux[] = {0};

	if (argc != 1)
		return -1;

	if ((max_processes = satoi(argv[0])) <= 0)
		return -1;

	/*
	 *	Esta parte cuando hagamos un test_processes con muchos procesos va a fallar porque va a saturar el stack
	 *	del proceso, por ejemplo, si probamos con 100000 procesos, y cada p_rq ocupa 16 bytes, entonces el stack
	 *	tendria que tener 1.6MB para almacenar ese array, pero solamente tenemos stacks de 4KB por proceso, por lo que
	 *	terminaria fallando y diria Failed to start "<comando>" cuando querramos ejecutar otro comando no built-in
	 *despues de correr el test. Una posible solución sería alocar el array así: p_rq *p_rqs = mem_alloc(max_processes *
	 *sizeof(p_rq)); Igualmente después nos diría que hubo un error al crear un proceso, y esta bien, porque al pedir
	 *mucha memoria el mem_alloc nos retorna NULL, lo que hace que el createProcess no sea exitoso y nos devuelva un PID
	 *= 0 generando el error esperado.
	 */
	p_rq p_rqs[max_processes];

	size_t counter = 0;

	while (1)
	{
		// Create max_processes processes
		for (rq = 0; rq < max_processes; rq++)
		{
			p_rqs[rq].pid = my_create_process("endless_loop", (mainFuncPtr) &endless_loop, 0, argvAux, defaultFds, 0);
			counter++;

			if (!p_rqs[rq].pid)
			{
				printf("test_processes: ERROR creating process\n");
				return -1;
			}
			else
			{
				// No es parte del test oficial, pero lo ponemos para que se pueda seguir escribiendo en la shell
				changeProcessPriority(p_rqs[rq].pid, 3);
				p_rqs[rq].state = RUNNING;
				alive++;
			}
		}

		// Randomly kills, blocks or unblocks processes until every one has been killed
		while (alive > 0)
		{
			for (rq = 0; rq < max_processes; rq++)
			{
				action = GetUniform(100) % 2;

				switch (action)
				{
					case 0:
						if (p_rqs[rq].state == RUNNING || p_rqs[rq].state == BLOCKED)
						{
							if (my_kill(p_rqs[rq].pid) == -1)
							{
								printf("test_processes: ERROR killing process\n");
								return -1;
							}
							p_rqs[rq].state = KILLED;
							alive--;
							waitPid(p_rqs[rq].pid);
						}
						break;

					case 1:
						if (p_rqs[rq].state == RUNNING)
						{
							if (my_block(p_rqs[rq].pid) == -1)
							{
								printf("test_processes: ERROR blocking process\n");
								return -1;
							}
							p_rqs[rq].state = BLOCKED;
						}
						break;
				}
			}

			// Randomly unblocks processes
			for (rq = 0; rq < max_processes; rq++)
				if (p_rqs[rq].state == BLOCKED && GetUniform(100) % 2)
				{
					if (my_unblock(p_rqs[rq].pid) == -1)
					{
						printf("test_processes: ERROR unblocking process\n");
						return -1;
					}
					p_rqs[rq].state = RUNNING;
				}
		}
	}
}
