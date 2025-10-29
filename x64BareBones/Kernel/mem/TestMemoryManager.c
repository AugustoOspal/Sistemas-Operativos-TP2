#include <stdlib.h>
#include <stdint.h>

#include "CuTest.h"
#include "pmem.h"
#include "TestMemoryManager.h"

#define TEST_POOL_SIZE (1024 * 1024)
#define PAGE_SIZE 4096

// ============ FORWARD DECLARATIONS ============

void testInitValid(CuTest *const tc);
void testAllocSinglePage(CuTest *const tc);
void testAllocSmallSize(CuTest *const tc);
void testAllocLargeSize(CuTest *const tc);
void testAllocZeroBytes(CuTest *const tc);
void testAllocOutOfMemory(CuTest *const tc);
void testAllocMultiple(CuTest *const tc);
void testAllocFragmentation(CuTest *const tc);
void testAllocBoundary(CuTest *const tc);
void testFreeMemory(CuTest *const tc);
void testFreeAndRealloc(CuTest *const tc);
void testFreeMultiplePages(CuTest *const tc);
void testStatsInitial(CuTest *const tc);
void testStatsAfterAlloc(CuTest *const tc);
void testStatsAfterFree(CuTest *const tc);

static uint8_t *testPool;
static void *allocPtr1;
static void *allocPtr2;
static void *allocPtr3;
static void *allocPtr4;
static pm_stats_t stats;
static size_t statsBefore_used;
static size_t statsBefore_free;

static inline void givenAMemoryPool(void) {
	testPool = (uint8_t *) malloc(TEST_POOL_SIZE);
}

static inline void whenMemoryManagerIsInitialized(void) {
	pm_init(testPool, TEST_POOL_SIZE);
}

static inline void whenMemoryIsAllocated(size_t size, void **result) {
	*result = mem_alloc(size);
}

static inline void whenMemoryIsFreed(void *ptr) {
	mem_free(ptr);
}

static inline void whenStatsAreRequested(void) {
	mem_get_stats(&stats);
}

static inline void whenStatsAreRecorded(void) {
	mem_get_stats(&stats);
	statsBefore_used = stats.used;
	statsBefore_free = stats.free;
}

static inline void thenPoolIsValid(CuTest *const tc) {
	CuAssertPtrNotNull(tc, testPool);
}

static inline void thenAllocationSucceeds(CuTest *const tc, void *ptr) {
	CuAssertPtrNotNull(tc, ptr);
}

static inline void thenAllocationFails(CuTest *const tc, void *ptr) {
	CuAssertPtrNull(tc, ptr);
}

static inline void thenAddressesAreDifferent(CuTest *const tc, void *ptr1, void *ptr2) {
	CuAssertTrue(tc, ptr1 != ptr2);
}

static inline void thenAddressesAreSame(CuTest *const tc, void *ptr1, void *ptr2) {
	CuAssertIntEquals(tc, (uintptr_t) ptr1, (uintptr_t) ptr2);
}

static inline void thenStatsTotal(CuTest *const tc) {
	CuAssertIntEquals(tc, TEST_POOL_SIZE, stats.total);
}

static inline void thenStatsUsedIsPositive(CuTest *const tc) {
	CuAssertTrue(tc, stats.used > 0);
}

static inline void thenStatsFreeIsLessThanTotal(CuTest *const tc) {
	CuAssertTrue(tc, stats.free < TEST_POOL_SIZE);
}

static inline void thenStatsAreConsistent(CuTest *const tc) {
	CuAssertIntEquals(tc, stats.total, stats.used + stats.free);
}

static inline void thenUsedMemoryIncreased(CuTest *const tc, size_t amount) {
	mem_get_stats(&stats);
	CuAssertIntEquals(tc, statsBefore_used + amount, stats.used);
}

static inline void thenFreeMemoryDecreased(CuTest *const tc, size_t amount) {
	mem_get_stats(&stats);
	CuAssertIntEquals(tc, statsBefore_free - amount, stats.free);
}

static inline void thenUsedMemoryDecreased(CuTest *const tc, size_t amount) {
	mem_get_stats(&stats);
	CuAssertIntEquals(tc, statsBefore_used - amount, stats.used);
}

static inline void thenFreeMemoryIncreased(CuTest *const tc, size_t amount) {
	mem_get_stats(&stats);
	CuAssertIntEquals(tc, statsBefore_free + amount, stats.free);
}

CuSuite *getMemoryManagerTestSuite(void) {
	CuSuite *const suite = CuSuiteNew();

	givenAMemoryPool();
	whenMemoryManagerIsInitialized();

	SUITE_ADD_TEST(suite, testInitValid);
	SUITE_ADD_TEST(suite, testAllocOutOfMemory);
	SUITE_ADD_TEST(suite, testAllocSinglePage);
	SUITE_ADD_TEST(suite, testAllocSmallSize);
	SUITE_ADD_TEST(suite, testAllocLargeSize);
	SUITE_ADD_TEST(suite, testAllocZeroBytes);
	SUITE_ADD_TEST(suite, testAllocMultiple);
	SUITE_ADD_TEST(suite, testAllocFragmentation);
	SUITE_ADD_TEST(suite, testFreeMemory);
	SUITE_ADD_TEST(suite, testFreeAndRealloc);
	SUITE_ADD_TEST(suite, testFreeMultiplePages);
	SUITE_ADD_TEST(suite, testStatsInitial);
	SUITE_ADD_TEST(suite, testStatsAfterAlloc);
	SUITE_ADD_TEST(suite, testStatsAfterFree);
	SUITE_ADD_TEST(suite, testAllocBoundary);

	return suite;
}

void testInitValid(CuTest *const tc) {
	thenPoolIsValid(tc);
}

void testAllocSinglePage(CuTest *const tc) {
	whenMemoryIsAllocated(PAGE_SIZE, &allocPtr1);
	thenAllocationSucceeds(tc, allocPtr1);
	whenMemoryIsFreed(allocPtr1);
}

void testAllocSmallSize(CuTest *const tc) {
	whenMemoryIsAllocated(100, &allocPtr1);
	thenAllocationSucceeds(tc, allocPtr1);
	whenMemoryIsFreed(allocPtr1);
}

void testAllocLargeSize(CuTest *const tc) {
	whenMemoryIsAllocated(100 * 1024, &allocPtr1);
	thenAllocationSucceeds(tc, allocPtr1);
	whenMemoryIsFreed(allocPtr1);
}

void testAllocZeroBytes(CuTest *const tc) {
	whenMemoryIsAllocated(0, &allocPtr1);
	thenAllocationSucceeds(tc, allocPtr1);
	whenMemoryIsFreed(allocPtr1);
}

void testAllocOutOfMemory(CuTest *const tc) {
	whenMemoryIsAllocated(TEST_POOL_SIZE + PAGE_SIZE, &allocPtr1);
	thenAllocationFails(tc, allocPtr1);
}

void testAllocMultiple(CuTest *const tc) {
	whenMemoryIsAllocated(PAGE_SIZE, &allocPtr1);
	whenMemoryIsAllocated(PAGE_SIZE, &allocPtr2);
	whenMemoryIsAllocated(PAGE_SIZE, &allocPtr3);

	thenAllocationSucceeds(tc, allocPtr1);
	thenAllocationSucceeds(tc, allocPtr2);
	thenAllocationSucceeds(tc, allocPtr3);

	thenAddressesAreDifferent(tc, allocPtr1, allocPtr2);
	thenAddressesAreDifferent(tc, allocPtr2, allocPtr3);

	whenMemoryIsFreed(allocPtr1);
	whenMemoryIsFreed(allocPtr2);
	whenMemoryIsFreed(allocPtr3);
}

void testAllocFragmentation(CuTest *const tc) {
	whenMemoryIsAllocated(PAGE_SIZE, &allocPtr1);
	whenMemoryIsAllocated(PAGE_SIZE, &allocPtr2);
	whenMemoryIsAllocated(PAGE_SIZE, &allocPtr3);

	whenMemoryIsFreed(allocPtr2);

	whenMemoryIsAllocated(PAGE_SIZE, &allocPtr4);
	thenAllocationSucceeds(tc, allocPtr4);
	thenAddressesAreSame(tc, allocPtr2, allocPtr4);

	whenMemoryIsFreed(allocPtr1);
	whenMemoryIsFreed(allocPtr3);
	whenMemoryIsFreed(allocPtr4);
}

void testFreeMemory(CuTest *const tc) {
	whenMemoryIsAllocated(PAGE_SIZE, &allocPtr1);
	thenAllocationSucceeds(tc, allocPtr1);
	whenMemoryIsFreed(allocPtr1);
	CuAssertTrue(tc, 1);
}

void testFreeAndRealloc(CuTest *const tc) {
	whenMemoryIsAllocated(PAGE_SIZE, &allocPtr1);
	whenMemoryIsFreed(allocPtr1);

	whenMemoryIsAllocated(PAGE_SIZE, &allocPtr2);
	thenAddressesAreSame(tc, allocPtr1, allocPtr2);

	whenMemoryIsFreed(allocPtr2);
}

void testFreeMultiplePages(CuTest *const tc) {
	whenMemoryIsAllocated(PAGE_SIZE * 5, &allocPtr1);
	thenAllocationSucceeds(tc, allocPtr1);
	whenMemoryIsFreed(allocPtr1);
}

void testStatsInitial(CuTest *const tc) {
	whenStatsAreRequested();

	thenStatsTotal(tc);
	thenStatsUsedIsPositive(tc);
	thenStatsFreeIsLessThanTotal(tc);
	thenStatsAreConsistent(tc);
}

void testStatsAfterAlloc(CuTest *const tc) {
	whenStatsAreRecorded();
	whenMemoryIsAllocated(PAGE_SIZE, &allocPtr1);

	thenUsedMemoryIncreased(tc, PAGE_SIZE);
	thenFreeMemoryDecreased(tc, PAGE_SIZE);

	mem_get_stats(&stats);
	thenStatsAreConsistent(tc);

	whenMemoryIsFreed(allocPtr1);
}

void testStatsAfterFree(CuTest *const tc) {
	whenMemoryIsAllocated(PAGE_SIZE, &allocPtr1);
	whenStatsAreRecorded();
	whenMemoryIsFreed(allocPtr1);

	thenUsedMemoryDecreased(tc, PAGE_SIZE);
	thenFreeMemoryIncreased(tc, PAGE_SIZE);

	mem_get_stats(&stats);
	thenStatsAreConsistent(tc);
}

void testAllocBoundary(CuTest *const tc) {
	whenMemoryIsAllocated(PAGE_SIZE - 1, &allocPtr1);
	whenMemoryIsAllocated(PAGE_SIZE + 1, &allocPtr2);
	whenMemoryIsAllocated(PAGE_SIZE * 2, &allocPtr3);

	thenAllocationSucceeds(tc, allocPtr1);
	thenAllocationSucceeds(tc, allocPtr2);
	thenAllocationSucceeds(tc, allocPtr3);

	whenMemoryIsFreed(allocPtr1);
	whenMemoryIsFreed(allocPtr2);
	whenMemoryIsFreed(allocPtr3);
}
