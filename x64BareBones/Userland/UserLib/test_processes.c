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
#include "./include/test_processes.h"
#include "./include/processes.h"
#include "./include/videoLib.h"

#define CIRCLE_RADIUS 50
#define BOX_SIZE 120

int test_processA(int argc, char *argv[])
{
	int radius = 5;
	int growing = 1;
	while (1)
	{
		drawRectangle(BOX_SIZE, BOX_SIZE, 0x000000, 40, 40);
		drawCircle(radius, 0xFF0000, 100, 100);

		if (growing)
		{
			radius += 2;
			if (radius >= CIRCLE_RADIUS)
				growing = 0;
		}
		else
		{
			radius -= 2;
			if (radius <= 5)
				growing = 1;
		}

		for (volatile int i = 0; i < 5000000; i++)
			;
	}
	return 0;
}

int test_processB(int argc, char *argv[])
{
	int radius = 5;
	int growing = 1;
	while (1)
	{
		// blockProcess(getPID());
		drawRectangle(BOX_SIZE, BOX_SIZE, 0x000000, 240, 40);
		drawCircle(radius, 0x00FF00, 300, 100);

		if (growing)
		{
			radius += 2;
			if (radius >= CIRCLE_RADIUS)
				growing = 0;
		}
		else
		{
			radius -= 2;
			if (radius <= 5)
				growing = 1;
		}

		for (volatile int i = 0; i < 5000000; i++)
			;
	}
	return 0;
}

int test_processC(int argc, char *argv[])
{
	int radius = 5;
	int growing = 1;
	while (1)
	{
		drawRectangle(BOX_SIZE, BOX_SIZE, 0x000000, 440, 40);
		drawCircle(radius, 0x0000FF, 500, 100);

		if (growing)
		{
			radius += 2;
			if (radius >= CIRCLE_RADIUS)
				growing = 0;
		}
		else
		{
			radius -= 2;
			if (radius <= 5)
				growing = 1;
		}

		for (volatile int i = 0; i < 5000000; i++)
			;
	}
	return 0;
}
