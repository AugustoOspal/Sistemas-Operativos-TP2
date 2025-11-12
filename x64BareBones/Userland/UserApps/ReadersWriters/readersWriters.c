#include "../UserLib/include/semaphores.h"
#include "../mem/include/pmem.h"
#include "../UserLib/include/processes.h"
#include <stdint.h>
#include <stdbool.h>

static char slot = 0;
static semaphoreP semFull;
static semaphoreP semEmpty;

typedef struct {
    int id;
    char symbol;
} writer_arg_t;

typedef struct {
    int id;
} reader_arg_t;

static void writerMain(void *arg) {
    writer_arg_t *a = (writer_arg_t *)arg;
    sys_sleepMilli(3000);
    semWait(semEmpty);
    slot = a->symbol;
    semPost(semFull);
    exit(0);
}

static void readerConsume (char value) {
    sys_write(1, &value, 1);
}

static void readerMain(void *arg) {
    reader_arg_t *a = (reader_arg_t *)arg;
    sys_sleepMilli(3000);
    semWait(semFull);
    char c = slot;
    slot = 0;
    semPost(semEmpty);
    readerConsume(c);
    exit(0);
}

int main(int argc, char **argv) {
    int W = (argc > 1) ? atoi(argv[1]) : 2;
    int R = (argc > 2) ? atoi(argv[2]) : 2;

    semEmpty = semOpen("mvar_empty", 1);
    semFull = semOpen("mvar_full",  0);

    static const char letters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    for (int i = 0; i < W; i++) {
        writer_arg_t *wa = (writer_arg_t *)mem_alloc(sizeof(writer_arg_t));
        wa->id = i;
        wa->symbol = letters[i % (int)(sizeof(letters)-1)];
        createProcess("Writer", writerMain, 0, NULL);
    }

    for (int j = 0; j < R; j++) {
        reader_arg_t *ra = (reader_arg_t *)mem_alloc(sizeof(reader_arg_t));
        ra->id = j;
        createProcess("Reader", readerMain, 0, NULL);
    }

    return 0;
}