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
		drawChar('A', 0xFFFFFF, 10, 10);
    }
}

static void procB(void *arg) {
    (void)arg;
    for (;;) {
        drawChar('B', 0x00FF00, 10, 11); //color verde
    }
}


int main()
{
	load_idt();
	kernel_memory_init();
	scheduler_init();

    proc_t *p1 = proc_create(procA, 0);
    proc_t *p2 = proc_create(procB, 0);
    if (!p1 || !p2) { sys_write(1, "proc_create fallo\n", 18); for(;;); }

	scheduler_add(p1);
	scheduler_add(p2);

	__asm__ __volatile__("int $0x20");
    for(;;);
	//return ((EntryPoint)shellAddress)();
}