#include "include/pipe.h"
#include "include/syscallLib.h"

int16_t pipeOpen(int16_t fds[2])
{
	return sys_pipe_open(fds);
}

int16_t pipeClose(const int pipe_id)
{
	return sys_pipe_close(pipe_id);
}