CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99 -fPIC
INCLUDE_DIRS = -I. \
               -I../Kernel/mem/include \
               -I../Kernel/lib/include \
               -I../Kernel/lib/ADT/Queue \
               -I../Kernel/lib/ADT/DoubleLinkedList \
               -I../Kernel/lib/string \
               -I../Kernel/sched/include \
               -I../Kernel/proc/include \
               -I../Kernel/sync/include \
               -I../Kernel/semaphore/include \
               -I../Kernel/ipc/include \
               -I../Kernel/include

KERNEL_TESTS = test_kernel

CUTEST_OBJ = CuTest.o
ALLTESTS_OBJ = AllTests.o
MOCK_OBJS = MockFunctions.o
TEST_OBJS = ../Kernel/mem/TestMemoryManager.o ../Kernel/semaphore/TestSemaphore.o ../Kernel/ipc/TestPipe.o
MM_OBJS = ../Kernel/mem/pmem_bitmap.o
ADT_OBJS = ../Kernel/lib/ADT/Queue/queue.o ../Kernel/lib/ADT/DoubleLinkedList/doubleLinkedList.o
LIB_OBJS = ../Kernel/lib/string/strings.o
SEM_OBJS = ../Kernel/semaphore/semaphore.o
PIPE_OBJS = ../Kernel/ipc/pipe.o

.PHONY: all test-kernel test-userland clean help

all: test-kernel

test-kernel: $(KERNEL_TESTS)
	@echo ""
	@echo "=========================================="
	@echo "Running Kernel Tests"
	@echo "=========================================="
	@./$(KERNEL_TESTS)

test_kernel: $(CUTEST_OBJ) $(ALLTESTS_OBJ) $(MOCK_OBJS) $(TEST_OBJS) $(MM_OBJS) $(ADT_OBJS) $(LIB_OBJS) $(SEM_OBJS) $(PIPE_OBJS)
	gcc -std=c99 -Wall -Wextra -g $(INCLUDE_DIRS) -o $@ $^ -lm

test-userland:
	@echo "Userland tests not yet implemented"

%.o: %.c
	gcc $(CFLAGS) $(INCLUDE_DIRS) -c $< -o $@

# Mock functions - shared across all test suites
MockFunctions.o: MockFunctions.c
	gcc $(CFLAGS) $(INCLUDE_DIRS) -c $< -o $@

# Kernel modules - compile with -fPIE for PIE executable
../Kernel/mem/TestMemoryManager.o: ../Kernel/mem/TestMemoryManager.c
	gcc $(CFLAGS) $(INCLUDE_DIRS) -c $< -o $@

../Kernel/mem/pmem_bitmap.o: ../Kernel/mem/pmem_bitmap.c
	gcc $(CFLAGS) $(INCLUDE_DIRS) -c $< -o $@

../Kernel/mem/pmem_buddy.o: ../Kernel/mem/pmem_buddy.c
	gcc $(CFLAGS) $(INCLUDE_DIRS) -c $< -o $@

../Kernel/lib/ADT/Queue/queue.o: ../Kernel/lib/ADT/Queue/queue.c
	gcc $(CFLAGS) $(INCLUDE_DIRS) -c $< -o $@

../Kernel/lib/ADT/DoubleLinkedList/doubleLinkedList.o: ../Kernel/lib/ADT/DoubleLinkedList/doubleLinkedList.c
	gcc $(CFLAGS) $(INCLUDE_DIRS) -c $< -o $@

../Kernel/lib/string/strings.o: ../Kernel/lib/string/strings.c
	gcc -Wall -Wextra -g -std=c99 -fPIE $(INCLUDE_DIRS) -c $< -o $@

../Kernel/semaphore/TestSemaphore.o: ../Kernel/semaphore/TestSemaphore.c
	gcc $(CFLAGS) $(INCLUDE_DIRS) -c $< -o $@

../Kernel/semaphore/semaphore.o: ../Kernel/semaphore/semaphore.c
	gcc $(CFLAGS) $(INCLUDE_DIRS) -c $< -o $@

../Kernel/ipc/TestPipe.o: ../Kernel/ipc/TestPipe.c
	gcc $(CFLAGS) $(INCLUDE_DIRS) -c $< -o $@

../Kernel/ipc/pipe.o: ../Kernel/ipc/pipe.c
	gcc $(CFLAGS) $(INCLUDE_DIRS) -c $< -o $@

clean:
	rm -f *.o test_kernel MockFunctions.o
	rm -f ../Kernel/mem/TestMemoryManager.o
	rm -f ../Kernel/mem/pmem_bitmap.o ../Kernel/mem/pmem_buddy.o
	rm -f ../Kernel/semaphore/TestSemaphore.o ../Kernel/semaphore/semaphore.o
	rm -f ../Kernel/ipc/TestPipe.o ../Kernel/ipc/pipe.o
	@echo "Test artifacts cleaned"

help:
	@echo "Test Framework"
	@echo ""
	@echo "Targets:"
	@echo "  make              - Build and run kernel tests"
	@echo "  make test-kernel  - Build and run kernel tests"
	@echo "  make test-userland - Build and run userland tests (TODO)"
	@echo "  make clean        - Remove test artifacts"
	@echo "  make help         - Show this help message"
