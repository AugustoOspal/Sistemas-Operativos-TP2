#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <stdbool.h>

sheduleInit();
shedule(void *);
addProcess();
removeProcess();
blockProcess();
unblockProcess();
getProcessInfo();
changeProcessPriority();
resignTimeWindow(); // usa yield

#endif
