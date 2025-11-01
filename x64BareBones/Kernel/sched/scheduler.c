#include "scheduler.h"

typedef enum ProcessState
{
    READY,
    RUNNING,
    BLOCKED,
    TERMINATED
}ProcessState;

typedef struct Process
{
    void *basePointer;
    void *stack;
    uint64_t pid;
    char *nombre;
    ProcessState state;
    uint8_t priority;
    struct Process *parent;
    uint8_t quantumLeft;
    bool foreground;
}Process;

switchProcess();
selectNextProcess();