// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "fonts.h"
#include "idtLoader.h"
#include "keyboardDriver.h"
#include "lib.h"
#include "moduleLoader.h"
#include "pmem.h"
#include "process.h"
#include "scheduler.h"
#include "soundDriver.h"
#include "timeLib.h"
#include "videoDriver.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// TODO: Despues borrar esto
#include "../semaphore/include/semaphore.h"
#include "test_processes.h"

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;
extern char _pm_pool_start;
extern char _pm_pool_end;

static const uint64_t PageSize = 0x1000;

static void *const shellAddress = (void *) 0x400000;
static void *const shellDataAddress = (void *) 0x500000;

typedef int (*EntryPoint)();

void clearBSS(void *bssAddress, uint64_t bssSize)
{
	memset(bssAddress, 0, bssSize);
}

void *getStackBase()
{
	return (void *) ((uint64_t) &endOfKernel + PageSize * 8 // The size of the stack itself, 32KiB
					 - sizeof(uint64_t)						// Begin at the top of the stack
	);
}

void *initializeKernelBinary()
{
	void *moduleAddresses[] = {shellAddress, shellDataAddress};

	loadModules(&endOfKernelBinary, moduleAddresses);
	clearBSS(&bss, &endOfKernel - &bss);

	return getStackBase();
}

static void kernel_memory_init(void)
{
	void *pool_start = (void *) &_pm_pool_start;
	size_t pool_len = (size_t) ((char *) &_pm_pool_end - (char *) &_pm_pool_start);
	mem_init(pool_start, pool_len);
}

int main()
{
	kernel_memory_init();
	initializeScheduler();
	semaphoresInit();
	keyboard_init();

	// Test de procesos
	// createProcess("Proceso A", processA, 0, NULL);
	// createProcess("Proceso B", processB, 0, NULL);
	// createProcess("Proceso C", processC, 0, NULL);
	const int16_t fds[] = {STDIN, STDOUT, STDERR};

	changeProcessPriority(createProcess("shell", shellAddress, 0, NULL, fds, false), 0);
	load_idt();
	// play_boot_sound();
	return 0;
}
