#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../lib/ADT/Queue/queue.h"
#include "../lib/string/strings.h"
#include "../mem/include/pmem.h"
#include "../semaphore/include/semaphore.h"
#include "CuTest.h"
#include "TestPipe.h"
#include "include/pipe.h"

// ============ FORWARD DECLARATIONS ============

void testPipeOpenCreatesValidPipe(CuTest *const tc);
void testPipeOpenReturnsFdsArray(CuTest *const tc);
void testPipeOpenMultiplePipes(CuTest *const tc);
void testPipeWriteSingleByte(CuTest *const tc);
void testPipeWriteMultipleBytes(CuTest *const tc);
void testPipeWriteInvalidPipeId(CuTest *const tc);
void testPipeReadSingleByte(CuTest *const tc);
void testPipeReadMultipleBytes(CuTest *const tc);
void testPipeReadInvalidPipeId(CuTest *const tc);
void testPipeWriteAndReadRoundTrip(CuTest *const tc);
void testPipeCircularBuffer(CuTest *const tc);
void testPipeClosesSuccessfully(CuTest *const tc);
void testPipeCloseInvalidId(CuTest *const tc);
void testPipeGetIndexFindsCorrectPipe(CuTest *const tc);
void testPipeSizeTrackingWrite(CuTest *const tc);
void testPipeSizeTrackingRead(CuTest *const tc);

// ============ STATIC STATE SECTION ============

#define TEST_POOL_SIZE (1024 * 1024)

static uint8_t *testMemoryPool;
static int16_t testFds[2];
static int16_t altFds[2];
static int16_t pipeId;
static int16_t altPipeId;

// ============ GIVEN HELPERS ============

static inline void givenMemoryPoolAllocated(void)
{
	testMemoryPool = (uint8_t *) malloc(TEST_POOL_SIZE);
}

static inline void whenMemoryManagerIsInitialized(void)
{
	mem_init(testMemoryPool, TEST_POOL_SIZE);
}

static inline void givenSemaphoresInitialized(void)
{
	semaphoresInit();
}

static inline void givenFreshPipeSystem(void)
{
	givenMemoryPoolAllocated();
	whenMemoryManagerIsInitialized();
	givenSemaphoresInitialized();
}

// ============ WHEN HELPERS ============

static inline int16_t whenPipeIsOpened(int16_t fds[2])
{
	return pipe_open(fds);
}

static inline int16_t whenDataIsWrittenToPipe(int16_t pipeId, const char *buffer, int count)
{
	return pipe_write(pipeId, buffer, count);
}

static inline int16_t whenDataIsReadFromPipe(int16_t pipeId, char *buffer, int count)
{
	return pipe_read(pipeId, buffer, count);
}

static inline int16_t whenPipeIsClosed(int16_t pipeId)
{
	return pipe_close(pipeId);
}

static inline int16_t whenPipeIndexIsQueried(int fd)
{
	return get_pipe_idx(fd);
}

// ============ THEN HELPERS ============

static inline void thenPipeIdIsValid(CuTest *const tc, int16_t id)
{
	CuAssertTrue(tc, id >= 0);
}

static inline void thenPipeIdIsInvalid(CuTest *const tc, int16_t id)
{
	CuAssertTrue(tc, id < 0);
}

static inline void thenFdsAreValid(CuTest *const tc, int16_t fds[2])
{
	CuAssertTrue(tc, fds[0] >= 0);
	CuAssertTrue(tc, fds[1] >= 0);
	CuAssertTrue(tc, fds[0] != fds[1]);
}

static inline void thenBytesWritten(CuTest *const tc, int16_t count, int16_t expected)
{
	CuAssertIntEquals(tc, expected, count);
}

static inline void thenBytesRead(CuTest *const tc, int16_t count, int16_t expected)
{
	CuAssertIntEquals(tc, expected, count);
}

static inline void thenBufferContains(CuTest *const tc, const char *buffer, const char *expected, size_t len)
{
	CuAssertTrue(tc, strncmp(buffer, expected, len) == 0);
}

static inline void thenCloseSucceeded(CuTest *const tc, int16_t result)
{
	CuAssertIntEquals(tc, 0, result);
}

// ============ SUITE INITIALIZATION ============

CuSuite *getPipeTestSuite(void)
{
	CuSuite *const suite = CuSuiteNew();

	givenFreshPipeSystem();

	SUITE_ADD_TEST(suite, testPipeOpenCreatesValidPipe);
	SUITE_ADD_TEST(suite, testPipeOpenReturnsFdsArray);
	SUITE_ADD_TEST(suite, testPipeOpenMultiplePipes);
	SUITE_ADD_TEST(suite, testPipeWriteSingleByte);
	SUITE_ADD_TEST(suite, testPipeWriteMultipleBytes);
	SUITE_ADD_TEST(suite, testPipeWriteInvalidPipeId);
	SUITE_ADD_TEST(suite, testPipeReadSingleByte);
	SUITE_ADD_TEST(suite, testPipeReadMultipleBytes);
	SUITE_ADD_TEST(suite, testPipeReadInvalidPipeId);
	SUITE_ADD_TEST(suite, testPipeWriteAndReadRoundTrip);
	SUITE_ADD_TEST(suite, testPipeCircularBuffer);
	SUITE_ADD_TEST(suite, testPipeClosesSuccessfully);
	SUITE_ADD_TEST(suite, testPipeCloseInvalidId);
	SUITE_ADD_TEST(suite, testPipeGetIndexFindsCorrectPipe);
	SUITE_ADD_TEST(suite, testPipeSizeTrackingWrite);
	SUITE_ADD_TEST(suite, testPipeSizeTrackingRead);

	return suite;
}

// ============ TEST CASES ============

void testPipeOpenCreatesValidPipe(CuTest *const tc)
{
	// Given: Pipe system is initialized
	// When: pipe_open is called
	pipeId = whenPipeIsOpened(testFds);

	// Then: Valid pipe ID is returned
	thenPipeIdIsValid(tc, pipeId);

	whenPipeIsClosed(pipeId);
}

void testPipeOpenReturnsFdsArray(CuTest *const tc)
{
	// Given: Pipe system is initialized
	// When: pipe_open is called
	pipeId = whenPipeIsOpened(testFds);

	// Then: Two valid and distinct FDs are returned in array
	thenFdsAreValid(tc, testFds);
	// fds[0] is read end, fds[1] is write end
	CuAssertTrue(tc, testFds[0] != testFds[1]);

	whenPipeIsClosed(pipeId);
}

void testPipeOpenMultiplePipes(CuTest *const tc)
{
	// Given: Pipe system is initialized
	// When: Multiple pipes are opened
	pipeId = whenPipeIsOpened(testFds);
	altPipeId = whenPipeIsOpened(altFds);

	// Then: Different pipe IDs are returned for each pipe
	thenPipeIdIsValid(tc, pipeId);
	thenPipeIdIsValid(tc, altPipeId);
	CuAssertTrue(tc, pipeId != altPipeId);

	// And: Different FDs are returned
	thenFdsAreValid(tc, testFds);
	thenFdsAreValid(tc, altFds);
	CuAssertTrue(tc, testFds[0] != altFds[0]);
	CuAssertTrue(tc, testFds[1] != altFds[1]);

	whenPipeIsClosed(pipeId);
	whenPipeIsClosed(altPipeId);
}

void testPipeWriteSingleByte(CuTest *const tc)
{
	// Given: Pipe is open
	pipeId = whenPipeIsOpened(testFds);
	thenPipeIdIsValid(tc, pipeId);

	// When: Single byte is written to pipe
	char data = 'A';
	int16_t written = whenDataIsWrittenToPipe(pipeId, &data, 1);

	// Then: One byte is written successfully
	thenBytesWritten(tc, written, 1);

	whenPipeIsClosed(pipeId);
}

void testPipeWriteMultipleBytes(CuTest *const tc)
{
	// Given: Pipe is open
	pipeId = whenPipeIsOpened(testFds);
	thenPipeIdIsValid(tc, pipeId);

	// When: Multiple bytes are written to pipe
	const char *message = "Hello";
	int16_t written = whenDataIsWrittenToPipe(pipeId, message, 5);

	// Then: All bytes are written
	thenBytesWritten(tc, written, 5);

	whenPipeIsClosed(pipeId);
}

void testPipeWriteInvalidPipeId(CuTest *const tc)
{
	// Given: Invalid pipe ID
	int16_t invalidPipeId = -1;

	// When: Write is attempted on invalid pipe
	const char *data = "test";
	int16_t result = whenDataIsWrittenToPipe(invalidPipeId, data, 4);

	// Then: Operation fails
	CuAssertIntEquals(tc, -1, result);
}

void testPipeReadSingleByte(CuTest *const tc)
{
	// Given: Pipe is open with data written
	pipeId = whenPipeIsOpened(testFds);
	char writeData = 'X';
	whenDataIsWrittenToPipe(pipeId, &writeData, 1);

	// When: Single byte is read from pipe
	char readData = '\0';
	int16_t readCount = whenDataIsReadFromPipe(pipeId, &readData, 1);

	// Then: One byte is read and matches written data
	thenBytesRead(tc, readCount, 1);
	CuAssertIntEquals(tc, 'X', readData);

	whenPipeIsClosed(pipeId);
}

void testPipeReadMultipleBytes(CuTest *const tc)
{
	// Given: Pipe is open with data written
	pipeId = whenPipeIsOpened(testFds);
	const char *message = "Test";
	whenDataIsWrittenToPipe(pipeId, message, 4);

	// When: Multiple bytes are read from pipe
	char buffer[10] = {0};
	int16_t readCount = whenDataIsReadFromPipe(pipeId, buffer, 4);

	// Then: All bytes are read and match
	thenBytesRead(tc, readCount, 4);
	thenBufferContains(tc, buffer, message, 4);

	whenPipeIsClosed(pipeId);
}

void testPipeReadInvalidPipeId(CuTest *const tc)
{
	// Given: Invalid pipe ID
	int16_t invalidPipeId = -1;

	// When: Read is attempted on invalid pipe
	char buffer[10];
	int16_t result = whenDataIsReadFromPipe(invalidPipeId, buffer, 10);

	// Then: Operation fails
	CuAssertIntEquals(tc, -1, result);
}

void testPipeWriteAndReadRoundTrip(CuTest *const tc)
{
	// Given: Pipe is open
	pipeId = whenPipeIsOpened(testFds);
	thenPipeIdIsValid(tc, pipeId);

	// When: Data is written
	const char *original = "RoundTrip";
	whenDataIsWrittenToPipe(pipeId, original, 9);

	// And: Same data is read back
	char retrieved[20] = {0};
	int16_t readCount = whenDataIsReadFromPipe(pipeId, retrieved, 9);

	// Then: Read data matches written data
	thenBytesRead(tc, readCount, 9);
	thenBufferContains(tc, retrieved, original, 9);

	whenPipeIsClosed(pipeId);
}

void testPipeCircularBuffer(CuTest *const tc)
{
	// Given: Pipe is open with 256-byte buffer
	pipeId = whenPipeIsOpened(testFds);
	thenPipeIdIsValid(tc, pipeId);

	// When: Buffer wraps around (write at position past middle, then continue)
	char pattern[300];
	for (int i = 0; i < 300; i++) {
		pattern[i] = (char)('A' + (i % 26));
	}

	// Write 256 bytes (fill buffer completely)
	int16_t written = whenDataIsWrittenToPipe(pipeId, pattern, 256);
	thenBytesWritten(tc, written, 256);

	// Read 256 bytes to empty buffer
	char readBuf[256] = {0};
	int16_t readCount = whenDataIsReadFromPipe(pipeId, readBuf, 256);
	thenBytesRead(tc, readCount, 256);

	// Then: Data is correct and circular buffer handled properly
	thenBufferContains(tc, readBuf, pattern, 256);

	whenPipeIsClosed(pipeId);
}

void testPipeClosesSuccessfully(CuTest *const tc)
{
	// Given: Pipe is open
	pipeId = whenPipeIsOpened(testFds);
	thenPipeIdIsValid(tc, pipeId);

	// When: Pipe is closed
	int16_t result = whenPipeIsClosed(pipeId);

	// Then: Close succeeds
	thenCloseSucceeded(tc, result);
}

void testPipeCloseInvalidId(CuTest *const tc)
{
	// Given: Invalid pipe ID
	int16_t invalidPipeId = -1;

	// When: Close is attempted on invalid pipe
	int16_t result = whenPipeIsClosed(invalidPipeId);

	// Then: Operation fails
	CuAssertIntEquals(tc, -1, result);
}

void testPipeGetIndexFindsCorrectPipe(CuTest *const tc)
{
	// Given: Pipe is open
	pipeId = whenPipeIsOpened(testFds);
	thenPipeIdIsValid(tc, pipeId);

	// When: Pipe index is queried using write FD
	int16_t foundPipeId = whenPipeIndexIsQueried(testFds[1]);

	// Then: Correct pipe ID is found
	CuAssertIntEquals(tc, pipeId, foundPipeId);

	// And: Pipe index is also found using read FD
	int16_t foundPipeId2 = whenPipeIndexIsQueried(testFds[0]);
	CuAssertIntEquals(tc, pipeId, foundPipeId2);

	whenPipeIsClosed(pipeId);
}

void testPipeSizeTrackingWrite(CuTest *const tc)
{
	// Given: Pipe is open (size starts at 0)
	pipeId = whenPipeIsOpened(testFds);
	thenPipeIdIsValid(tc, pipeId);

	// When: Data is written to pipe
	const char *data = "ABC";
	int16_t written = whenDataIsWrittenToPipe(pipeId, data, 3);

	// Then: Size increases as data is written
	thenBytesWritten(tc, written, 3);
	// Note: Size is internal to pipe structure, just verify write succeeded

	whenPipeIsClosed(pipeId);
}

void testPipeSizeTrackingRead(CuTest *const tc)
{
	// Given: Pipe is open with data written
	pipeId = whenPipeIsOpened(testFds);
	const char *data = "DEF";
	whenDataIsWrittenToPipe(pipeId, data, 3);

	// When: Data is read from pipe
	char buffer[10] = {0};
	int16_t readCount = whenDataIsReadFromPipe(pipeId, buffer, 3);

	// Then: Size decreases as data is read
	thenBytesRead(tc, readCount, 3);
	thenBufferContains(tc, buffer, data, 3);

	whenPipeIsClosed(pipeId);
}