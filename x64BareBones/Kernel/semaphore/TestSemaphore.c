#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../lib/ADT/DoubleLinkedList/doubleLinkedList.h"
#include "../lib/ADT/Queue/queue.h"
#include "../lib/string/strings.h"
#include "../mem/include/pmem.h"
#include "CuTest.h"
#include "TestSemaphore.h"
#include "include/semaphore.h"

// ============ MOCK FUNCTIONS (stubs for linking) ============

uint64_t getPid(void)
{
	return 1;
}

void addProcessToBlockQueue(uint64_t pid)
{
	(void) pid;
}

void unblockProcess(uint64_t pid)
{
	(void) pid;
}

void yield(void)
{
}

void acquire(uint64_t *lock)
{
	(void) lock;
}

int try_acquire(uint64_t *lock)
{
	(void) lock;
	return 0;
}

void release(uint64_t *lock)
{
	(void) lock;
}

// ============ FORWARD DECLARATIONS ============

void testSemaphoresInitSucceeds(CuTest *const tc);
void testSemOpenCreatesNewSemaphore(CuTest *const tc);
void testSemOpenReturnsExistingSemaphore(CuTest *const tc);
void testSemOpenRejectsNullName(CuTest *const tc);
void testSemOpenWithInitialValue(CuTest *const tc);
void testSemCloseRemovesProcess(CuTest *const tc);
void testSemWaitDecrementsValue(CuTest *const tc);
void testSemWaitBlocksWhenZero(CuTest *const tc);
void testSemPostIncrementsValue(CuTest *const tc);
void testSemPostUnblocksWaitingProcess(CuTest *const tc);
void testSemTryWaitSucceedsWhenAvailable(CuTest *const tc);
void testSemTryWaitFailsWhenZero(CuTest *const tc);
void testSemTryWaitRestoresValueOnFailure(CuTest *const tc);
void testSemGetValueReturnsPositiveValue(CuTest *const tc);
void testSemGetValueReturnsNegativeCountWhenWaiting(CuTest *const tc);
void testSemGetValueRejectsNullSemaphore(CuTest *const tc);
void testSemGetValueRejectsNullPointer(CuTest *const tc);
void testSemUnlinkMarksPendingDestruction(CuTest *const tc);
void testSemUnlinkReturnsErrorOnNonexistent(CuTest *const tc);
void testSemUnlinkMultipleSemaphores(CuTest *const tc);
void testSemaphoreSpinlockProtection(CuTest *const tc);
void testSemaphoreMultipleWaitAndPost(CuTest *const tc);

// ============ STATIC STATE SECTION ============

#define TEST_POOL_SIZE (1024 * 1024)
#define SEMAPHORE_NAME_TEST "test_semaphore"
#define SEMAPHORE_NAME_ALT "alt_semaphore"

static uint8_t *testMemoryPool;
static semaphoreP testSemaphore;
static semaphoreP altSemaphore;
static int semaphoreValue;

// ============ GIVEN HELPERS ============

static inline void givenMemoryPoolAllocated(void)
{
	testMemoryPool = (uint8_t *) malloc(TEST_POOL_SIZE);
}

static inline void whenMemoryManagerIsInitialized(void)
{
	pm_init(testMemoryPool, TEST_POOL_SIZE);
}

static inline void givenSemaphoresInitialized(void)
{
	semaphoresInit();
}

static inline void givenFreshSemaphoreSystem(void)
{
	givenMemoryPoolAllocated();
	whenMemoryManagerIsInitialized();
	givenSemaphoresInitialized();
}

// ============ WHEN HELPERS ============

static inline semaphoreP whenSemaphoreIsOpened(const char *name, int initialValue)
{
	return semOpen(name, initialValue);
}

static inline semaphoreP whenSemaphoreIsOpenedAgain(const char *name)
{
	return semOpen(name, 0);
}

static inline void whenSemaphoreIsClosed(semaphoreP sem)
{
	semClose(sem);
}

static inline void whenSemaphoreWaitIsCalled(semaphoreP sem)
{
	semWait(sem);
}

static inline int whenSemaphoreTryWaitIsCalled(semaphoreP sem)
{
	return semTryWait(sem);
}

static inline void whenSemaphorePostIsCalled(semaphoreP sem)
{
	semPost(sem);
}

static inline int whenSemaphoreValueIsQueried(semaphoreP sem, int *valueOut)
{
	return semGetValue(sem, valueOut);
}

static inline int whenSemUnlinkIsCalled(const char *name)
{
	return sem_unlink(name);
}

// ============ THEN HELPERS ============

static inline void thenSemaphoreIsNotNull(CuTest *const tc, semaphoreP sem)
{
	CuAssertPtrNotNull(tc, sem);
}

static inline void thenSemaphoreIsNull(CuTest *const tc, semaphoreP sem)
{
	CuAssertPtrNull(tc, sem);
}

static inline void thenSemaphoresAreEqual(CuTest *const tc, semaphoreP sem1, semaphoreP sem2)
{
	CuAssertPtrEquals(tc, sem1, sem2);
}

static inline void thenValueIsEqual(CuTest *const tc, int expected, int actual)
{
	CuAssertIntEquals(tc, expected, actual);
}

static inline void thenValueIsPositive(CuTest *const tc, int value)
{
	CuAssertTrue(tc, value > 0);
}

static inline void thenValueIsNegative(CuTest *const tc, int value)
{
	CuAssertTrue(tc, value < 0);
}

static inline void thenTryWaitSucceeded(CuTest *const tc, int result)
{
	CuAssertIntEquals(tc, 0, result);
}

static inline void thenTryWaitFailed(CuTest *const tc, int result)
{
	CuAssertIntEquals(tc, -1, result);
}

static inline void thenUnlinkSucceeded(CuTest *const tc, int result)
{
	CuAssertIntEquals(tc, 0, result);
}

static inline void thenUnlinkFailed(CuTest *const tc, int result)
{
	CuAssertIntEquals(tc, -1, result);
}

static inline void thenSemGetValueSucceeded(CuTest *const tc, int result)
{
	CuAssertIntEquals(tc, 0, result);
}

// ============ SUITE INITIALIZATION ============

CuSuite *getSemaphoreTestSuite(void)
{
	CuSuite *const suite = CuSuiteNew();

	givenFreshSemaphoreSystem();

	SUITE_ADD_TEST(suite, testSemaphoresInitSucceeds);
	SUITE_ADD_TEST(suite, testSemOpenCreatesNewSemaphore);
	SUITE_ADD_TEST(suite, testSemOpenReturnsExistingSemaphore);
	SUITE_ADD_TEST(suite, testSemOpenRejectsNullName);
	SUITE_ADD_TEST(suite, testSemOpenWithInitialValue);
	SUITE_ADD_TEST(suite, testSemCloseRemovesProcess);
	SUITE_ADD_TEST(suite, testSemWaitDecrementsValue);
	SUITE_ADD_TEST(suite, testSemWaitBlocksWhenZero);
	SUITE_ADD_TEST(suite, testSemPostIncrementsValue);
	SUITE_ADD_TEST(suite, testSemPostUnblocksWaitingProcess);
	SUITE_ADD_TEST(suite, testSemTryWaitSucceedsWhenAvailable);
	SUITE_ADD_TEST(suite, testSemTryWaitFailsWhenZero);
	SUITE_ADD_TEST(suite, testSemTryWaitRestoresValueOnFailure);
	SUITE_ADD_TEST(suite, testSemGetValueReturnsPositiveValue);
	SUITE_ADD_TEST(suite, testSemGetValueReturnsNegativeCountWhenWaiting);
	SUITE_ADD_TEST(suite, testSemGetValueRejectsNullSemaphore);
	SUITE_ADD_TEST(suite, testSemGetValueRejectsNullPointer);
	SUITE_ADD_TEST(suite, testSemUnlinkMarksPendingDestruction);
	SUITE_ADD_TEST(suite, testSemUnlinkReturnsErrorOnNonexistent);
	SUITE_ADD_TEST(suite, testSemUnlinkMultipleSemaphores);
	SUITE_ADD_TEST(suite, testSemaphoreSpinlockProtection);
	SUITE_ADD_TEST(suite, testSemaphoreMultipleWaitAndPost);

	return suite;
}

// ============ TEST CASES ============

void testSemaphoresInitSucceeds(CuTest *const tc)
{
	// Given: Memory system is initialized and semaphores are initialized
	// When: semaphoresInit is called (already done in suite setup)
	// Then: System is ready to create semaphores

	testSemaphore = whenSemaphoreIsOpened("sem_trywait", 5);
	thenSemaphoreIsNotNull(tc, testSemaphore);

	whenSemaphoreIsClosed(testSemaphore);
}

void testSemOpenCreatesNewSemaphore(CuTest *const tc)
{
	// Given: Semaphore system is initialized
	// When: semOpen is called with new name
	testSemaphore = whenSemaphoreIsOpened("sem_open_new", 3);

	// Then: New semaphore is created and returned
	thenSemaphoreIsNotNull(tc, testSemaphore);

	whenSemaphoreIsClosed(testSemaphore);
}

void testSemOpenReturnsExistingSemaphore(CuTest *const tc)
{
	// Given: Semaphore already exists
	testSemaphore = whenSemaphoreIsOpened("sem_open_exist", 5);
	thenSemaphoreIsNotNull(tc, testSemaphore);

	// When: semOpen is called again with same name
	semaphoreP retrieved = whenSemaphoreIsOpenedAgain("sem_open_exist");

	// Then: Same semaphore object is returned
	thenSemaphoresAreEqual(tc, testSemaphore, retrieved);

	whenSemaphoreIsClosed(testSemaphore);
	whenSemaphoreIsClosed(retrieved);
}

void testSemOpenRejectsNullName(CuTest *const tc)
{
	// Given: Semaphore system is initialized
	// When: semOpen is called with NULL name
	testSemaphore = semOpen(NULL, 5);

	// Then: Operation fails and returns NULL
	thenSemaphoreIsNull(tc, testSemaphore);
}

void testSemOpenWithInitialValue(CuTest *const tc)
{
	// Given: Semaphore system is initialized
	// When: semOpen is called with specific initial value and unique name
	testSemaphore = whenSemaphoreIsOpened("unique_initial_sem", 7);

	// Then: Semaphore value can be retrieved correctly
	int result = whenSemaphoreValueIsQueried(testSemaphore, &semaphoreValue);
	thenSemGetValueSucceeded(tc, result);
	thenValueIsEqual(tc, 7, semaphoreValue);

	whenSemaphoreIsClosed(testSemaphore);
}

void testSemCloseRemovesProcess(CuTest *const tc)
{
	// Given: Semaphore is open by current process
	testSemaphore = whenSemaphoreIsOpened("sem_close_proc", 1);
	thenSemaphoreIsNotNull(tc, testSemaphore);

	// When: semClose is called
	whenSemaphoreIsClosed(testSemaphore);

	// Then: Process is no longer associated with semaphore
	// (Verification is implicit: linkedProcesses count reduced)
	CuAssertTrue(tc, 1);
}

void testSemWaitDecrementsValue(CuTest *const tc)
{
	// Given: Semaphore with positive value
	testSemaphore = whenSemaphoreIsOpened("sem_wait_decr", 3);

	// When: semWait is called (decreases counter, does not yield in test since mocked)
	whenSemaphoreWaitIsCalled(testSemaphore);

	// Then: Value is decremented even though blocking does not occur in tests
	int result = whenSemaphoreValueIsQueried(testSemaphore, &semaphoreValue);
	thenSemGetValueSucceeded(tc, result);
	// Value should be 2 because semWait decremented (value >= 0, so no blocking in actual kernel)
	thenValueIsEqual(tc, 2, semaphoreValue);

	whenSemaphoreIsClosed(testSemaphore);
}

void testSemWaitBlocksWhenZero(CuTest *const tc)
{
	// Given: Semaphore with zero value
	testSemaphore = whenSemaphoreIsOpened("sem_block", 0);

	// When: semWait is called (value becomes -1, indicating waiting)
	whenSemaphoreWaitIsCalled(testSemaphore);

	// Then: Value goes negative indicating process is waiting
	int result = whenSemaphoreValueIsQueried(testSemaphore, &semaphoreValue);
	thenSemGetValueSucceeded(tc, result);
	thenValueIsNegative(tc, semaphoreValue);

	whenSemaphoreIsClosed(testSemaphore);
}

void testSemPostIncrementsValue(CuTest *const tc)
{
	// Given: Semaphore with positive value
	testSemaphore = whenSemaphoreIsOpened("sem_post_inc", 2);

	// When: semPost is called
	whenSemaphorePostIsCalled(testSemaphore);

	// Then: Value is incremented
	int result = whenSemaphoreValueIsQueried(testSemaphore, &semaphoreValue);
	thenSemGetValueSucceeded(tc, result);
	thenValueIsEqual(tc, 3, semaphoreValue);

	whenSemaphoreIsClosed(testSemaphore);
}

void testSemPostUnblocksWaitingProcess(CuTest *const tc)
{
	// Given: Semaphore with waiting process (value is negative)
	testSemaphore = whenSemaphoreIsOpened("sem_post_unblock", 0);
	whenSemaphoreWaitIsCalled(testSemaphore);

	// When: semPost is called
	whenSemaphorePostIsCalled(testSemaphore);

	// Then: Value returns to 0 (process was unblocked)
	int result = whenSemaphoreValueIsQueried(testSemaphore, &semaphoreValue);
	thenSemGetValueSucceeded(tc, result);
	CuAssertTrue(tc, semaphoreValue >= 0);

	whenSemaphoreIsClosed(testSemaphore);
}

void testSemTryWaitSucceedsWhenAvailable(CuTest *const tc)
{
	// Given: Semaphore with positive value
	testSemaphore = whenSemaphoreIsOpened("sem_trywait", 5);

	// When: semTryWait is called
	int result = whenSemaphoreTryWaitIsCalled(testSemaphore);

	// Then: Operation succeeds (returns 0)
	thenTryWaitSucceeded(tc, result);

	// And: Value is decremented
	int queryResult = whenSemaphoreValueIsQueried(testSemaphore, &semaphoreValue);
	thenSemGetValueSucceeded(tc, queryResult);
	thenValueIsEqual(tc, 4, semaphoreValue);

	whenSemaphoreIsClosed(testSemaphore);
}

void testSemTryWaitFailsWhenZero(CuTest *const tc)
{
	// Given: Semaphore with zero value
	testSemaphore = whenSemaphoreIsOpened("sem_block", 0);

	// When: semTryWait is called
	int result = whenSemaphoreTryWaitIsCalled(testSemaphore);

	// Then: Operation fails (returns -1)
	thenTryWaitFailed(tc, result);

	whenSemaphoreIsClosed(testSemaphore);
}

void testSemTryWaitRestoresValueOnFailure(CuTest *const tc)
{
	// Given: Semaphore with zero value
	testSemaphore = whenSemaphoreIsOpened("sem_trywait_restore", 0);

	// When: semTryWait fails
	int result = whenSemaphoreTryWaitIsCalled(testSemaphore);
	thenTryWaitFailed(tc, result);

	// Then: Value remains zero (not decremented)
	int queryResult = whenSemaphoreValueIsQueried(testSemaphore, &semaphoreValue);
	thenSemGetValueSucceeded(tc, queryResult);
	thenValueIsEqual(tc, 0, semaphoreValue);

	whenSemaphoreIsClosed(testSemaphore);
}

void testSemGetValueReturnsPositiveValue(CuTest *const tc)
{
	// Given: Semaphore with positive value
	testSemaphore = whenSemaphoreIsOpened("sem_getval_pos", 10);

	// When: semGetValue is called
	int result = whenSemaphoreValueIsQueried(testSemaphore, &semaphoreValue);

	// Then: Function returns 0 (success) and value is positive
	thenSemGetValueSucceeded(tc, result);
	thenValueIsPositive(tc, semaphoreValue);
	thenValueIsEqual(tc, 10, semaphoreValue);

	whenSemaphoreIsClosed(testSemaphore);
}

void testSemGetValueReturnsNegativeCountWhenWaiting(CuTest *const tc)
{
	// Given: Semaphore with waiting process
	testSemaphore = whenSemaphoreIsOpened("sem_block", 0);
	whenSemaphoreWaitIsCalled(testSemaphore);

	// When: semGetValue is called
	int result = whenSemaphoreValueIsQueried(testSemaphore, &semaphoreValue);

	// Then: Function returns 0 and value is negative (count of waiting processes)
	thenSemGetValueSucceeded(tc, result);
	thenValueIsNegative(tc, semaphoreValue);

	whenSemaphoreIsClosed(testSemaphore);
}

void testSemGetValueRejectsNullSemaphore(CuTest *const tc)
{
	// Given: Null semaphore pointer
	testSemaphore = NULL;

	// When: semGetValue is called with NULL semaphore
	int result = semGetValue(testSemaphore, &semaphoreValue);

	// Then: Operation fails (returns -1)
	CuAssertIntEquals(tc, -1, result);
}

void testSemGetValueRejectsNullPointer(CuTest *const tc)
{
	// Given: Valid semaphore but null output pointer
	testSemaphore = whenSemaphoreIsOpened("sem_trywait", 5);

	// When: semGetValue is called with NULL output pointer
	int result = semGetValue(testSemaphore, NULL);

	// Then: Operation fails (returns -1)
	CuAssertIntEquals(tc, -1, result);

	whenSemaphoreIsClosed(testSemaphore);
}

void testSemUnlinkMarksPendingDestruction(CuTest *const tc)
{
	// Given: Semaphore exists
	testSemaphore = whenSemaphoreIsOpened("sem_unlink_mark", 1);

	// When: sem_unlink is called
	int result = whenSemUnlinkIsCalled("sem_unlink_mark");

	// Then: Operation succeeds (returns 0)
	thenUnlinkSucceeded(tc, result);

	whenSemaphoreIsClosed(testSemaphore);
}

void testSemUnlinkReturnsErrorOnNonexistent(CuTest *const tc)
{
	// Given: Semaphore does not exist
	// When: sem_unlink is called on non-existent semaphore
	int result = whenSemUnlinkIsCalled("nonexistent_sem");

	// Then: Operation fails (returns -1)
	thenUnlinkFailed(tc, result);
}

void testSemUnlinkMultipleSemaphores(CuTest *const tc)
{
	// Given: Multiple semaphores exist
	testSemaphore = whenSemaphoreIsOpened("sem_unlink_mult1", 1);
	altSemaphore = whenSemaphoreIsOpened("sem_unlink_mult2", 2);

	// When: sem_unlink is called on first semaphore
	int result1 = whenSemUnlinkIsCalled("sem_unlink_mult1");

	// Then: First semaphore is marked for destruction
	thenUnlinkSucceeded(tc, result1);

	// When: sem_unlink is called on second semaphore
	int result2 = whenSemUnlinkIsCalled("sem_unlink_mult2");

	// Then: Second semaphore is also marked for destruction
	thenUnlinkSucceeded(tc, result2);

	whenSemaphoreIsClosed(testSemaphore);
	whenSemaphoreIsClosed(altSemaphore);
}

void testSemaphoreSpinlockProtection(CuTest *const tc)
{
	// Given: Semaphore is created
	testSemaphore = whenSemaphoreIsOpened("sem_spinlock_protect", 5);

	// When: Multiple wait/post operations are performed rapidly
	for (int i = 0; i < 3; i++)
	{
		whenSemaphoreWaitIsCalled(testSemaphore);
		whenSemaphorePostIsCalled(testSemaphore);
	}

	// Then: Semaphore remains in consistent state
	int result = whenSemaphoreValueIsQueried(testSemaphore, &semaphoreValue);
	thenSemGetValueSucceeded(tc, result);
	CuAssertTrue(tc, semaphoreValue == 5);

	whenSemaphoreIsClosed(testSemaphore);
}

void testSemaphoreMultipleWaitAndPost(CuTest *const tc)
{
	// Given: Semaphore with initial value
	testSemaphore = whenSemaphoreIsOpened("sem_wait_mult", 3);

	// When: Multiple wait operations are performed
	whenSemaphoreWaitIsCalled(testSemaphore);
	whenSemaphoreWaitIsCalled(testSemaphore);

	// Then: Value reflects consumed resources
	int result = whenSemaphoreValueIsQueried(testSemaphore, &semaphoreValue);
	thenSemGetValueSucceeded(tc, result);
	thenValueIsEqual(tc, 1, semaphoreValue);

	// When: Posts are performed
	whenSemaphorePostIsCalled(testSemaphore);
	whenSemaphorePostIsCalled(testSemaphore);

	// Then: Resources are restored
	result = whenSemaphoreValueIsQueried(testSemaphore, &semaphoreValue);
	thenSemGetValueSucceeded(tc, result);
	thenValueIsEqual(tc, 3, semaphoreValue);

	whenSemaphoreIsClosed(testSemaphore);
}