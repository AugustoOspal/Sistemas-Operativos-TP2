include Makefile.inc

KERNEL=kernel.bin

# Find all C source files in subsystems (excluding test files)
SOURCES=$(wildcard core/*.c) \
         $(wildcard interrupt/*.c) \
         $(wildcard drivers/*/*.c) \
         $(wildcard lib/*/*.c) \
         $(wildcard syscall/*.c) \
         $(filter-out mem/Test%.c,$(wildcard mem/*.c)) \
         $(wildcard sched/*.c) \
         $(wildcard proc/*.c)

# Loader (entry point) - must be first
LOADER=arch/x86_64/asm/loader.asm
LOADER_OBJ=$(LOADER:.asm=.o)

# Find all other ASM files in architecture directory (excluding loader)
SOURCES_ASM=$(filter-out $(LOADER),$(wildcard arch/x86_64/asm/*.asm))

# Convert to object files
OBJECTS=$(SOURCES:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)

# Linker script
LDSCRIPT=arch/x86_64/boot/kernel.ld

# Include directories
INCLUDE_DIRS=-I./arch/x86_64/include \
             -I./core/include \
             -I./interrupt/include \
             -I./drivers/include \
             -I./drivers/video \
             -I./drivers/keyboard \
             -I./drivers/sound \
             -I./drivers/timer \
             -I./lib/include \
             -I./lib/console \
             -I./lib/string \
             -I./syscall/include \
             -I./arch/x86_64/asm \
             -I./mem/include \
             -I./sched/include \
             -I./proc/include

# Updated GCCFLAGS with include directories
GCC_WITH_INCLUDES=$(GCCFLAGS) $(INCLUDE_DIRS)

.PHONY: all clean

all: $(KERNEL)

$(KERNEL): $(LOADER_OBJ) $(OBJECTS) $(OBJECTS_ASM)
	$(LD) $(LDFLAGS) -T $(LDSCRIPT) -o $(KERNEL) $(LOADER_OBJ) $(OBJECTS) $(OBJECTS_ASM)

%.o: %.c
	$(GCC) $(GCC_WITH_INCLUDES) -c $< -o $@

%.o: %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	find . -name "*.o" -delete
	rm -f $(KERNEL)

