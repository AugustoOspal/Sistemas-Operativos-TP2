include Makefile.inc

# TODO: Ver si se puede emprolijar pasando cosas al Makefile.inc
# TODO: Aca no hizo falta poner -no-pie en LDFLAGS porque usamos LD directamente, ver si se puede cambiar en el makefile de la terminal

KERNEL_ELF=kernel.elf
KERNEL_BIN=kernel.bin

# Find all C source files in subsystems (excluding test files)
SOURCES=$(wildcard core/*.c) \
         $(wildcard interrupt/*.c) \
         $(wildcard drivers/*/*.c) \
         $(wildcard lib/*/*.c) \
         $(wildcard lib/*/*/*.c) \
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
             -I./lib/ADT/Queue \
             -I./lib/ADT/DoubleLinkedList \
             -I./syscall/include \
             -I./arch/x86_64/asm \
             -I./mem/include \
             -I./sched/include \
             -I./proc/include \
             -I./proc

# Updated GCCFLAGS with include directories
GCC_WITH_INCLUDES=$(GCCFLAGS) $(INCLUDE_DIRS)

.PHONY: all clean

all: $(KERNEL_BIN) $(KERNEL_ELF)

# Linkeo para binario (usa OUTPUT_FORMAT del linker script)
$(KERNEL_BIN): $(LOADER_OBJ) $(OBJECTS) $(OBJECTS_ASM)
	$(LD) $(LDFLAGS) -T $(LDSCRIPT) -o $(KERNEL_BIN) $(LOADER_OBJ) $(OBJECTS) $(OBJECTS_ASM)

# Linkeo para ELF (ignora OUTPUT_FORMAT del linker script con --oformat)
$(KERNEL_ELF): $(LOADER_OBJ) $(OBJECTS) $(OBJECTS_ASM)
	$(LD) $(LDFLAGS) -T $(LDSCRIPT) --oformat=elf64-x86-64 -o $(KERNEL_ELF) $(LOADER_OBJ) $(OBJECTS) $(OBJECTS_ASM)

%.o: %.c
	$(GCC) $(GCC_WITH_INCLUDES) -c $< -o $@

%.o: %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	find . -name "*.o" -delete
	rm -f $(KERNEL_ELF) $(KERNEL_BIN)

