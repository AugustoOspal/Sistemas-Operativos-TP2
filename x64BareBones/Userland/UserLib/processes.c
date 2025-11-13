#include "include/processes.h"
#include "include/usrio.h"
#include <stdbool.h>
#include <stdlib.h>

/*
 *	@brie
 */
uint64_t createProcess(const char *name, int (*main)(int argc, char *argv[]), const int argc, char *argv[],
					   int16_t fds[FD_AMOUNT], const bool foreground)
{
	return sys_createProcess(name, main, argc, argv, fds, foreground);
}

void deleteProcess(uint64_t pid)
{
	sys_deleteProcess(pid);
}

uint64_t getPID()
{
	return sys_getPid();
}

uint64_t getProcessesInfo(char *buffer, uint64_t bufferSize)
{
	return sys_getAllProcessesInfo(buffer, bufferSize);
}

void changeProcessPriority(uint64_t pid, uint8_t newPriority)
{
	sys_changeProcessPriority(pid, newPriority);
}

void blockProcess(uint64_t pid)
{
	sys_blockProcess(pid);
}

void unblockProcess(uint64_t pid)
{
	sys_unblockProcess(pid);
}

void yield()
{
	sys_yield();
}

uint64_t waitPid(uint64_t pid)
{
	return sys_waitPid(pid);
}

void cat()
{
	int charsInline = 0;
	char c;
	while ((c = getchar()) != '\0')
	{
		if (c == '\b')
		{
			if (charsInline > 0)
			{
				charsInline--;
				putchar(c);
			}
		}
		else
		{
			if (c == '\n')
			{
				charsInline = 0;
			}
			else
			{
				charsInline++;
			}
			putchar(c);
		}
	}
}

void loop()
{
	int pid = sys_getPid();
	while (1)
	{
		printf("Hi! My PID is: %d\n", pid);
		sys_sleepMilli(2000);
	}
}

void wc()
{
	int chars = 0;
	int words = 0;
	int lines = 0;
	char c;
	bool inWord = false;

	while ((c = getchar()) != '\0')
	{
		chars++;
		if (c == '\n')
		{
			lines++;
			inWord = false;
		}
		else if (c == ' ' || c == '\t')
		{
			inWord = false;
		}
		else if (!inWord)
		{
			words++;
			inWord = true;
		}
	}
	printf("%d %d %d\n", lines, words, chars);
}

void filter()
{
	char c;
	while ((c = getchar()) != '\0')
	{
		if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' || c == 'A' || c == 'E' || c == 'I' || c == 'O' ||
			c == 'U')
		{
			putchar(c);
		}
	}
}

void kill(uint64_t pid)
{
	sys_deleteProcess(pid);
}
