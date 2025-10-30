#include "lib.h"
#include <stdint.h>
#include <string.h>
#include "fonts.h"
#include "videoDriver.h"
#include "keyboardDriver.h"
#include "moduleLoader.h"
#include "idtLoader.h"
#include "timeLib.h"
#include "soundDriver.h"
#include "pmem.h"
#include "switch_context.h"
#include "process.h"
#include "scheduler.h"
#include "syscalls.h"

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;
extern char _pm_pool_start;
extern char _pm_pool_end;

static const uint64_t PageSize = 0x1000;

static void * const shellAddress = (void*)0x400000;
static void * const shellDataAddress = (void*)0x500000;

typedef int (*EntryPoint)();


void clearBSS(void * bssAddress, uint64_t bssSize)
{
	memset(bssAddress, 0, bssSize);
}

void * getStackBase()
{
	return (void*)(
		(uint64_t)&endOfKernel
		+ PageSize * 8				//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
	);
}

void * initializeKernelBinary()
{
	void * moduleAddresses[] = {
		shellAddress,
		shellDataAddress
	};

	loadModules(&endOfKernelBinary, moduleAddresses);
	clearBSS(&bss, &endOfKernel - &bss);

	return getStackBase();
}

static void kernel_memory_init(void) {
    void  *pool_start = (void*)&_pm_pool_start;
    size_t pool_len = (size_t)((char*)&_pm_pool_end - (char*)&_pm_pool_start);
    pm_init(pool_start, pool_len);
}



static void procA(void *arg) {
    (void)arg;
    for (;;) {
		sys_write(1, "A", 1);
    }
}

static void procB(void *arg) {
    (void)arg;
    for (;;) {
        sys_write(1, "B", 1);
    }
}


int main()
{
	load_idt();
	kernel_memory_init();
	play_boot_sound();
    sys_write(1, "Lanzando proceso...\n", 21);

    proc_t *p1 = proc_create(procA, 0);
    if (!p1) { sys_write(1, "proc_create fallo\n", 18); for(;;); }

    start_process(p1->saved_rsp);

    // Si vuelve, algo falló:
    sys_write(1, "No deberia volver\n", 18);
    for(;;);
	//return ((EntryPoint)shellAddress)();
}
