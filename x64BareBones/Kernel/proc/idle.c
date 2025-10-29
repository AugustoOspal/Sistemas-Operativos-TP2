#include "include/process.h"
#include <stdint.h>

static void idle_main(void *args);
extern void _hlt(void); 

// Puntero global al proceso idle
static proc_t *idle_proc = NULL;

// Crea el proceso idle
proc_t *idle_init(void) {
	char *name = "idle";
	idle_proc = proc_create(idle_main, NULL);

    if (idle_proc == NULL) { 
        while (1) {
            _hlt(); // 
        }
    }
    return idle_proc;
}

static void idle_main(void *args) {
    (void)args; 

    for (;;) {
        _hlt(); // duerme el cpu hasta que haya una interrupción
    }
}

proc_t *get_idle_proc(void) {
    return idle_proc;
}
