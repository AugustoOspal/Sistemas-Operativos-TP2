#ifndef _SYSCALL_LIB_H
#define _SYSCALL_LIB_H

#include "timeLib.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct
{
	uint64_t total;
	uint64_t used;
	uint64_t free;
} pm_stats_t;

#define FD_AMOUNT 3

// Video
extern void sys_clearScreen(void);
extern void sys_putPixel(uint32_t hexColor, uint64_t x, uint64_t y);
extern void sys_drawChar(char c, uint32_t hexColor, uint64_t x, uint64_t y);
extern void sys_drawString(const char *str, uint32_t hexColor, uint64_t x, uint64_t y);
extern void sys_drawRectangle(uint64_t width, uint64_t height, uint32_t hexColor, uint64_t x, uint64_t y);
extern void sys_drawCircle(uint64_t radius, uint32_t hexColor, uint64_t x, uint64_t y);
extern void sys_drawDecimal(uint64_t value, uint32_t hexColor, uint64_t x, uint64_t y);
extern void sys_drawHexa(uint64_t value, uint32_t hexColor, uint64_t x, uint64_t y);
extern void sys_drawBin(uint64_t value, uint32_t hexColor, uint64_t x, uint64_t y);
extern uint16_t sys_getScreenWidth(void);
extern uint16_t sys_getScreenHeight(void);

// Teclado.
// TODO: Esto habria que sacarlo creo porque ya tenemos sys_read
extern char sys_kbdGetChar(void);

// Shell
extern uint64_t sys_write(uint8_t fd, const char *str, uint64_t count);
extern uint64_t sys_read(uint8_t fd, char *buffer, uint64_t count);

extern void sys_zoomIn(void);
extern void sys_zoomOut(void);

extern void sys_getDateTime(dateTime *dt);
extern uint64_t sys_getRegisters(uint64_t *registers);
extern void sys_sleepMilli(uint64_t milliseconds);

// Sonido
extern void sys_playSound(uint32_t frequency, uint32_t duration);

// Procesos
extern uint64_t sys_createProcess(const char *name, int (*main)(int argc, char *argv[]), int argc, char *argv[],
								  int16_t fds[FD_AMOUNT], bool foreground);
extern void sys_deleteProcess(uint64_t pid);
extern uint64_t sys_getPid();
extern uint64_t sys_getAllProcessesInfo(char *buffer, uint64_t bufferSize);
extern void sys_changeProcessPriority(uint64_t pid, uint8_t newPriority);
extern void sys_blockProcess(uint64_t pid);
extern void sys_unblockProcess(uint64_t pid);
extern void sys_yield();
extern uint64_t sys_waitPid(uint64_t pid);
extern int16_t sys_getProcessFd(int std);

// Semaforos
extern void *sys_semOpen(const char *name, int value);
extern void sys_semClose(void *sem);
extern void sys_semWait(void *sem);
extern void sys_semPost(void *sem);
extern int sys_semTryWait(void *sem);
extern int sys_semGetValue(void *sem, int *sval);
extern int sys_semUnlink(const char *name);

// Pipes
extern int16_t sys_pipe_open(int16_t fds[2]);
extern int16_t sys_pipe_close(int16_t pipe_id);

// Memoria
extern void sys_mem_info(pm_stats_t *stats);
extern void *sys_mem_alloc(size_t size);
extern void sys_mem_free(void *ptr);

// Excepciones
extern void opCodeException();

#endif
