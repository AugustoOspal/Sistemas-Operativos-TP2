#include <stdint.h>

#include "time.h"
#include "keyboardDriver.h"
#include "scheduler.h"

static void int_20(TrapFrame * tf);
static void int_21();

void irqDispatcher(uint64_t irq, TrapFrame * tf) {

	switch (irq) {

		// Timer Tick
		case 0:
			int_20(tf);
			break;

		// Teclado
		case 1:
			int_21();
			break;
	}
	return;
}

void int_20(TrapFrame * tf) {
	timer_handler();
	scheduler_on_tick(tf);
}

void int_21(){
	keyboard_handler();
}
