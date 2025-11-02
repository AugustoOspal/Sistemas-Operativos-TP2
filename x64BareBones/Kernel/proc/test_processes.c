// TODO: Despues borrar todo esto

#include "./include/test_processes.h"

#include "../drivers/include/videoDriver.h"

/*
 *  El volatile es para que el compilador no lo optimice. Asi se cuelga ahi
 */

int processA(int argc, char *argv[])
{
	int counter = 0;
	while (1)
	{
		drawRectangle(50, 50, 0xFF0000, 10, 10);
		drawString("A:", 0xFFFFFF, 20, 20);
		drawDecimal(counter++, 0xFFFFFF, 40, 20);

		for (volatile int i = 0; i < 5000000; i++)
			;
	}
	return 0;
}

int processB(int argc, char *argv[])
{
	int counter = 0;
	while (1)
	{
		drawRectangle(50, 50, 0x00FF00, 70, 10);
		drawString("B:", 0xFFFFFF, 80, 20);
		drawDecimal(counter++, 0xFFFFFF, 100, 20);

		for (volatile int i = 0; i < 5000000; i++)
			;
	}
	return 0;
}

int processC(int argc, char *argv[])
{
	int counter = 0;
	while (1)
	{
		drawRectangle(50, 50, 0x0000FF, 130, 10);
		drawString("C:", 0xFFFFFF, 140, 20);
		drawDecimal(counter++, 0xFFFFFF, 160, 20);

		for (volatile int i = 0; i < 5000000; i++)
			;
	}
	return 0;
}