#include "include/pipe.h"
#include "../lib/string/strings.h"
#include "../mem/include/pmem.h"
#include "../semaphore/include/semaphore.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

typedef struct
{
	char buffer[PIPE_BUFFER];
	int16_t fds[PIPE_FDS_AMOUNT];
	uint64_t readingPos;
	uint64_t writePos;
	uint64_t size;

	char writeSemName[SEM_NAME_SIZE];
	char readSemName[SEM_NAME_SIZE];
} pipe_t;

pipe_t *pipes[MAX_PIPES];
static uint16_t nextFd = 0;

static void buildSemName(int pipeId, const char *suffix, char semName[SEM_NAME_SIZE]);

int16_t pipe_open(int16_t fds[2])
{
	for (int pipeID = 0; pipeID < MAX_PIPES; pipeID++)
	{
		if (pipes[pipeID] == NULL)
		{
			pipes[pipeID] = mem_alloc(sizeof(pipe_t));

			if (pipes[pipeID] == NULL)
			{
				return -1;
			}

			pipes[pipeID]->readingPos = 0;
			pipes[pipeID]->writePos = 0;
			pipes[pipeID]->size = 0;

			pipes[pipeID]->fds[0] = nextFd++;
			fds[0] = pipes[pipeID]->fds[0];
			pipes[pipeID]->fds[1] = nextFd++;
			fds[1] = pipes[pipeID]->fds[1];

			buildSemName(pipeID, "write", pipes[pipeID]->writeSemName);
			const semaphoreP writeSem = semOpen(pipes[pipeID]->writeSemName, PIPE_BUFFER);
			if (writeSem == NULL)
			{
				mem_free(pipes[pipeID]);
				pipes[pipeID] = NULL;
				return -1;
			}

			// Semáforo de lectura
			buildSemName(pipeID, "read", pipes[pipeID]->readSemName);
			const semaphoreP readSem = semOpen(pipes[pipeID]->readSemName, 0);
			if (readSem == NULL)
			{
				sem_unlink(pipes[pipeID]->writeSemName);
				semClose(readSem);
				mem_free(pipes[pipeID]);
				pipes[pipeID] = NULL;
				return -1;
			}

			return pipeID;
		}
	}

	return -1;
}

int16_t pipe_write(const int pipe_id, const char *buffer, const int count)
{
	if (pipe_id < 0 || pipe_id >= MAX_PIPES || pipes[pipe_id] == NULL)
		return -1;

	pipe_t *pipe = pipes[pipe_id];

	int written = 0;

	for (; written < count; written++)
	{
		semWait(getSem(pipe->writeSemName));

		pipe->buffer[pipe->writePos] = buffer[written];
		pipe->writePos = (pipe->writePos + 1) % PIPE_BUFFER;
		pipe->size++;

		semPost(getSem(pipe->readSemName));
	}

	return written;
}

int16_t pipe_read(const int pipe_id, char *buffer, const int count)
{
	if (pipe_id < 0 || pipe_id >= MAX_PIPES || pipes[pipe_id] == NULL)
		return -1;

	pipe_t *pipe = pipes[pipe_id];

	int read = 0;

	for (; read < count; read++)
	{
		semWait(getSem(pipe->readSemName));

		buffer[read] = pipe->buffer[pipe->readingPos];
		pipe->readingPos = (pipe->readingPos + 1) % PIPE_BUFFER;
		pipe->size--;

		semPost(getSem(pipe->writeSemName));
	}

	return read;
}

int16_t pipe_close(const int pipe_id)
{
	if (pipe_id < 0 || pipe_id >= MAX_PIPES || pipes[pipe_id] == NULL)
		return -1;

	pipe_t *pipe = pipes[pipe_id];

	semClose(getSem(pipe->readSemName));
	semClose(getSem(pipe->writeSemName));

	sem_unlink(pipe->readSemName);
	sem_unlink(pipe->writeSemName);

	mem_free(pipe);
	pipes[pipe_id] = NULL;

	return 0;
}

static void buildSemName(const int pipeId, const char *suffix, char semName[SEM_NAME_SIZE])
{
	ksprintf(semName, "%d_%s", pipeId, suffix);
}

int16_t get_pipe_idx(const int16_t fd)
{
	for (int pipeId = 0; pipeId < MAX_PIPES; pipeId++)
	{
		if (pipes[pipeId] == NULL)
			continue;

		if (pipes[pipeId]->fds[0] == fd || pipes[pipeId]->fds[1] == fd)
			return pipeId;
	}

	return -1;
}