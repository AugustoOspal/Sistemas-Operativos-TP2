#ifndef _PIPES_H_
#define _PIPES_H_

#include "../../semaphore/include/semaphore.h"
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>

#define MAX_PIPES INT16_MAX
#define MAX_BUFFER 256
#define SEM_NAME_SIZE 32
#define BASIC_FDS 3 // 0,1,2 reservados para stdin, stdout, stderr
#define PIPE_FDS_AMOUNT 2

int16_t generateFileDescriptor(void);
int16_t pipe_open(int16_t fds[2]);
int16_t pipe_write(int pipe_id, const char *buffer, int count);
int16_t pipe_read(int pipe_id, char *buffer, int count);
int16_t pipe_close(int pipe_id);

#endif