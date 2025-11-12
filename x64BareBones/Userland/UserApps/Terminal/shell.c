#include "shell.h"

#include <stdlib.h>

#include "processes.h"

#define BUFFER 500
#define MAX_PIPE_CMDS 8
#define MAX_COMMAND_ARGS 8
static int defaultFds[] = {STDIN, STDOUT, STDERR};
#define SPECIAL_KEY_MAX_VALUE 5

typedef struct
{
	command_entry *entry;
	int argc;
	char **argv;
} parsed_command_t;

static char *argv_storage[MAX_PIPE_CMDS][MAX_COMMAND_ARGS + 2];

static command_entry commands[] = {
	// COMANDOS BUILTIN
	{"help", CMD_BUILTIN, help},
	{"exception 1", CMD_BUILTIN, exception_1},
	{"exception 2", CMD_BUILTIN, exception_2},
	{"pongisgolf", CMD_BUILTIN, startPongis},
	{"zoom in", CMD_BUILTIN, zoom_in},
	{"zoom out", CMD_BUILTIN, zoom_out},
	{"clear", CMD_BUILTIN, clear_screen},
	{"date", CMD_BUILTIN, printDateTime},
	{"registers", CMD_BUILTIN, getRegisters},
	{"busywait", CMD_BUILTIN, busy_wait},
	{"busywaitkernel", CMD_BUILTIN, busy_wait_kernel},
	{"exit", CMD_BUILTIN, exitShell},

	// COMANDOS PROCESOS
	{"ps", CMD_PROC, runPs}, 
	{"cat", CMD_PROC, runCat},
	{"mem", CMD_PROC, runMem},
	{"loop", CMD_PROC, runLoop},
	{"wc", CMD_PROC, runWc},
	{"filter", CMD_PROC, runFilter}
	
};

#define COMMAND_COUNT (sizeof(commands) / sizeof(command_entry))

static bool isWhitespace(char c);
static char *skipSpaces(char *ptr);
static void rtrim(char *str);
static char *trimSegment(char *segment);
static bool matchesCommandName(char *segment, const char *name, char **argsStart);
static int fillArgVector(parsed_command_t *parsed, char *argsStart);
static int parseCommandSegment(char *segment, parsed_command_t *parsed, char **argvStorage);
static int parseEntry(char *input, parsed_command_t *outCommands, int maxCommands);
static void executeSingleCommand(parsed_command_t *cmd);
static void executePipeline(parsed_command_t *cmds, int count);

void show_prompt()
{
	printf("user@itba:> ");
}

static uint8_t active = 1;

void startShell()
{
	char input_buffer[BUFFER];
	parsed_command_t entryCommands[MAX_PIPE_CMDS];

	while (active)
	{
		show_prompt();
		readInput(input_buffer);
		to_lower(input_buffer);

		int commandCount = parseEntry(input_buffer, entryCommands, MAX_PIPE_CMDS);
		if (commandCount <= 0)
		{
			continue;
		}

		if (commandCount == 1)
		{
			executeSingleCommand(&entryCommands[0]);
		}
		else
		{
			executePipeline(entryCommands, commandCount);
		}
	}
}

//usar la de stringLib
static int str_len(const char *str)
{
	int len = 0;
	if (str == NULL)
		return 0;
	while (str[len] != '\0')
		len++;
	return len;
}

static bool isWhitespace(char c)
{
	return (c == ' ' || c == '\t');
}

static char *skipSpaces(char *ptr)
{
	while (ptr && *ptr != '\0' && isWhitespace(*ptr))
		ptr++;
	return ptr;
}

static void rtrim(char *str)
{
	if (str == NULL)
		return;

	int len = str_len(str);
	while (len > 0 && isWhitespace(str[len - 1]))
	{
		str[len - 1] = '\0';
		len--;
	}
}

static char *trimSegment(char *segment)
{
	char *trimmed = skipSpaces(segment);
	rtrim(trimmed);
	return trimmed;
}

static bool matchesCommandName(char *segment, const char *name, char **argsStart)
{
	char *cursor = segment;
	const char *target = name;

	while (*target && *cursor && *cursor == *target)
	{
		cursor++;
		target++;
	}

	if (*target != '\0')
	{
		return false;
	}

	if (*cursor == '\0')
	{
		*argsStart = cursor;
		return true;
	}

	if (isWhitespace(*cursor))
	{
		*cursor = '\0';
		cursor++;
		cursor = skipSpaces(cursor);
		*argsStart = cursor;
		return true;
	}

	return false;
}

static int fillArgVector(parsed_command_t *parsed, char *argsStart)
{
	int argc = 0;
	parsed->argv[argc++] = parsed->entry->name;

	if (argsStart != NULL)
	{
		char *cursor = argsStart;
		while (*cursor != '\0')
		{
			cursor = skipSpaces(cursor);
			if (*cursor == '\0')
			{
				break;
			}

			if (argc - 1 >= MAX_COMMAND_ARGS)
			{
				printf("Too many arguments. Max supported: %d\n", MAX_COMMAND_ARGS);
				return -1;
			}

			parsed->argv[argc++] = cursor;
			while (*cursor != '\0' && !isWhitespace(*cursor))
				cursor++;

			if (*cursor == '\0')
			{
				break;
			}

			*cursor = '\0';
			cursor++;
		}
	}

	parsed->argv[argc] = NULL;
	parsed->argc = argc;
	return 0;
}

static int parseCommandSegment(char *segment, parsed_command_t *parsed, char **argvStorage)
{
	char *argsStart = NULL;
	command_entry *entry = NULL;

	for (int i = 0; i < COMMAND_COUNT; i++)
	{
		if (matchesCommandName(segment, commands[i].name, &argsStart))
		{
			entry = &commands[i];
			break;
		}
	}

	if (entry == NULL)
	{
		notACommand(segment);
		return -1;
	}

	parsed->entry = entry;
	parsed->argv = argvStorage;
	return fillArgVector(parsed, argsStart);
}

static int parseEntry(char *input, parsed_command_t *outCommands, int maxCommands)
{
	int count = 0; // numero de comandos parseados
	char *cursor = input; // cursor de parseo
	bool expectCommand = true; // si espero un comando o un pipe

	while (expectCommand)
	{
		char *segmentStart = skipSpaces(cursor);
		if (*segmentStart == '\0') // fin del input
		{
			if (count == 0)
			{
				return 0;
			}

			if (expectCommand) // esperaba un comando pero no hay mas
			{
				printf("Empty command in pipeline.\n");
				return -1;
			}

			return count;
		}

		if (count >= maxCommands)
		{
			printf("Pipeline too long. Max commands: %d\n", maxCommands);
			return -1;
		}

		char *scan = segmentStart;
		while (*scan != '\0' && *scan != '|') 
			scan++;

		char delimiter = *scan;
		char *next = (delimiter == '|') ? scan + 1 : scan;// next apunta al comienzo del próximo segmento
		if (delimiter == '|')
		{
			*scan = '\0';
		}

		char *cleanSegment = trimSegment(segmentStart);
		if (*cleanSegment == '\0')
		{
			printf("Empty command in pipeline.\n");
			return -1;
		}

		if (parseCommandSegment(cleanSegment, &outCommands[count], argv_storage[count]) != 0)
		{
			return -1;
		}

		count++;
		expectCommand = (delimiter == '|');
		cursor = next;

		if (!expectCommand)
		{
			break;
		}
	}

	return count;
}

static void executeSingleCommand(parsed_command_t *cmd)
{
	if (cmd->entry->type == CMD_BUILTIN)
	{
		cmd->entry->function(cmd->argc, cmd->argv);
		return;
	}

	uint64_t pid = createProcess(cmd->entry->name, cmd->entry->function, cmd->argc, cmd->argv, defaultFds);
	if (pid == 0)
	{
		printf("Failed to start \"%s\".\n", cmd->entry->name);
		return;
	}
	waitPid(pid);
}

static void executePipeline(parsed_command_t *cmds, int count)
{
	for (int i = 0; i < count; i++)
	{
		if (cmds[i].entry->type != CMD_PROC)
		{
			printf("Pipelines only support process commands. \"%s\" is builtin.\n", cmds[i].entry->name);
			return;
		}
	}

	int pipeCount = count - 1;
	int pipeIds[MAX_PIPE_CMDS - 1];
	int pipeFds[MAX_PIPE_CMDS - 1][2];
	uint64_t pids[MAX_PIPE_CMDS];

	for (int i = 0; i < pipeCount; i++)
	{
		pipeIds[i] = sys_pipe_open(pipeFds[i]);
		if (pipeIds[i] < 0)
		{
			printf("Failed to create pipe %d.\n", i);
			for (int j = 0; j < i; j++)
				sys_pipe_close(pipeIds[j]);
			return;
		}
	}

	int spawned = 0;
	for (; spawned < count; spawned++)
	{
		int fds[FD_AMOUNT];
		fds[STDIN] = (spawned == 0) ? defaultFds[STDIN] : pipeFds[spawned - 1][0];
		fds[STDOUT] = (spawned == count - 1) ? defaultFds[STDOUT] : pipeFds[spawned][1];
		fds[STDERR] = defaultFds[STDERR];

		pids[spawned] = createProcess(cmds[spawned].entry->name, cmds[spawned].entry->function, cmds[spawned].argc, cmds[spawned].argv, fds);
		if (pids[spawned] == 0)
		{
			printf("Failed to start \"%s\".\n", cmds[spawned].entry->name);
			break;
		}
	}

	for (int i = 0; i < spawned; i++)
	{
		waitPid(pids[i]);
	}

	for (int i = 0; i < pipeCount; i++)
	{
		sys_pipe_close(pipeIds[i]);
	}
}
// Falta implementar la syscall read
void readInput(char *buffer)
{
	char *c = buffer;
	int limit_count = 0;

	do
	{
		*c = getchar();
		if (*c <= SPECIAL_KEY_MAX_VALUE)
		{
			c--;
		}
		else if (*c == '\b')
		{
			if (c > buffer)
			{
				putchar(*c);
				c--;
			}
			c--;
		}
		else
		{
			putchar(*c);
			limit_count++;
			if (limit_count > BUFFER)
				break;
		}
	} while ((*c++) != '\n');
	*(c - 1) = '\0';
}

// Imprime todos los comandos disponibles
int help(int argc, char *argv[])
{
	printf("Commands:\n");
	for (int i = 0; i < COMMAND_COUNT; i++)
	{
		printf("\t- %s\n", commands[i].name);
	}
	return 0;
}

// Faltan las syscals del dateTime
int printDateTime(int argc, char *argv[])
{
	dateTime dt;
	getDateTime(&dt);
	printf(" %d/%d/%d %d:%d:%d\n", dt.day, dt.month, dt.year, dt.hour, dt.min, dt.sec);
	return 0;
}

// Nombres de los registros para el comando 'registers'
// El orden DEBE COINCIDIR con la estructura Registers_t en syscalls.h
static const char *register_names[] = {"R15", "R14", "R13", "R12", "R11", "R10", "R9 ", "R8 ", "RSI", "RDI",
									   "RBP", "RDX", "RCX", "RBX", "RAX", "RIP", "CS ", "FLG", "RSP", "SS "};

int getRegisters(int argc, char *argv[])
{
	uint64_t registers[20];
	if (get_regist(registers) != 0)
	{
		printf("No snapshot available. Press Ctrl+R first.\n");
	}
	else
	{
		printf("Register snapshot:\n");
		for (int i = 0; i < 20; i++)
		{
			printf("  %s: %x\n", register_names[i], registers[i]);
		}
	}
	return 0;
}

int startPongis(int argc, char *argv[])
{
	printf("Starting Pongis Golf...\n");
	startPongisGolf();
	clearScreen();
	return 0;
}

void notACommand(char *input)
{
	printf("Command \"%s\" not found. Type 'help' for a list of commands.\n", input);
}

int clear_screen(int argc, char *argv[])
{
	clearScreen();
	return 0;
}

int zoom_in(int argc, char *argv[])
{
	zoomIn();
	return 0;
}

int zoom_out(int argc, char *argv[])
{
	zoomOut();
	return 0;
}

int exitShell(int argc, char *argv[])
{
	printf("\n");
	printf("Exiting...\n");

	printf("\n[Exit succesful]\n");
	active = 0;
	return 0;
}

int exception_1(int argc, char *argv[])
{
	int a = 1 / 0;
	(void) a;
	return 0;
}

int exception_2(int argc, char *argv[])
{
	opCodeException();
	return 0;
}

/*
	Esto es exclusivamente para provar que cuando
	hacemos el ctrl + r dentro de userland el rip
	va a estar en la parte de userland y no en kernel.
	Porque con los juegos o la terminal esta siempre en
	el kernel esperando una tecla, y con sleep tambien
	lo mismo.
*/
int busy_wait(int argc, char *argv[])
{
	printf("Running userland busy-wait. Press Ctrl+R now.\n");

	// El volatile es para que el compilador no lo optimice
	volatile uint64_t counter = 0;
	for (uint64_t i = 0; i < 1000000000; i++)
	{
		counter += i;
	}

	printf("Userland busy-wait finished.\n");
	return 0;
}

int busy_wait_kernel(int argc, char *argv[])
{
	printf("Running kernel busy-wait. Press Ctrl+R now.\n");

	sleepMilli(5000);

	printf("Kernel busy-wait finished.\n");
	return 0;
}

//Comandos no Builtin

int runPs(int argc, char *argv[])
{
	char buffer[2000];
	getProcessesInfo(buffer, 2000);
	printf("%s", buffer);
	return 0;
}

int runCat(int argc, char *argv[])
{
	cat();
	return 0;
}

int runMem(int argc, char *argv[])
{
	pm_stats_t stats;
	get_mem_info(&stats);
	printf("Memory stats:\n");
	printf("  Total: %d bytes\n", stats.total);
	printf("  Used:  %d bytes\n", stats.used);
	printf("  Free:  %d bytes\n", stats.free);
	return 0;
}

int runLoop(int argc, char *argv[])
{
	loop();
	return 0;
}

int runWc(int argc, char *argv[])
{
	wc();
	return 0;
}

int runFilter(int argc, char *argv[])
{
	filter();
	return 0;
}

int main()
{
	startShell();
	return 0;
}
