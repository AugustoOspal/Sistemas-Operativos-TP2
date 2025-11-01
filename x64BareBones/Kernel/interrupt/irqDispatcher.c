#include <stdint.h>

#include "time.h"
#include "keyboardDriver.h"
#include "scheduler.h"
#include "process.h"

static void int_20(TrapFrame * tf);
static void int_21(TrapFrame * tf);

void irqDispatcher(TrapFrame * tf, uint64_t irq) {

	switch (irq) {

		// Timer Tick
		case 0:
			int_20(tf);
			break;

		// Teclado
		case 1:
			int_21(tf);
			break;
	}
	return;
}

void int_20(TrapFrame * tf) {
	timer_handler();
}

void int_21(TrapFrame * tf){
	keyboard_handler(tf);
}
