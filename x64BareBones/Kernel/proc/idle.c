#include "../interrupt/include/interrupts.h"
#include "include/process.h"

int idleMain(int argc, char *argv[])
{
	while (1)
	{
		_hlt();
	}

	return 0;
}

void createIdleProcess()
{
	createProcess(idleMain, 0, NULL);
}