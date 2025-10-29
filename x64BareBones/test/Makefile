CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99 -fPIC
INCLUDE_DIRS = -I. \
               -I../Kernel/mem/include \
               -I../Kernel/lib/include \
               -I../Kernel/include

KERNEL_TESTS = test_kernel

CUTEST_OBJ = CuTest.o
ALLTESTS_OBJ = AllTests.o
TEST_OBJS = ../Kernel/mem/TestMemoryManager.o
MM_OBJS = ../Kernel/mem/pmem_bitmap.o ../Kernel/mem/pmem_buddy.o

.PHONY: all test-kernel test-userland clean help

all: test-kernel

test-kernel: $(KERNEL_TESTS)
	@echo ""
	@echo "=========================================="
	@echo "Running Kernel Tests"
	@echo "=========================================="
	@./$(KERNEL_TESTS)

test_kernel: $(CUTEST_OBJ) $(ALLTESTS_OBJ) $(TEST_OBJS) $(MM_OBJS)
	gcc -std=c99 -Wall -Wextra -g $(INCLUDE_DIRS) -o $@ $^ -lm

test-userland:
	@echo "Userland tests not yet implemented"

%.o: %.c
	gcc $(CFLAGS) $(INCLUDE_DIRS) -c $< -o $@

../Kernel/mem/TestMemoryManager.o: ../Kernel/mem/TestMemoryManager.c
	gcc $(CFLAGS) $(INCLUDE_DIRS) -c $< -o $@

clean:
	rm -f *.o test_kernel
	rm -f ../Kernel/mem/TestMemoryManager.o
	rm -f ../Kernel/mem/pmem_bitmap.o ../Kernel/mem/pmem_buddy.o
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
