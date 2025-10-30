include ../../Makefile.inc

MODULE_ELF := shell.elf
MODULE_BIN := 0000-shell.bin
LDSCRIPT   := shell.ld
OUTPUT_DIR := ../..

MY_CFLAGS  := $(GCCFLAGS) \
               -Ilib \
               -I../../UserLib/include \
               -I../PongisGolf/lib

CSOURCES   := $(wildcard [^_]*.c)
COBJECTS   := $(CSOURCES:.c=.o)
ASMSOURCES := $(wildcard *.asm)
ASMOBJS    := $(ASMSOURCES:.asm:.o)
LOADER_O   := _loader.o

.PHONY: all clean

all: $(OUTPUT_DIR)/$(MODULE_BIN) $(OUTPUT_DIR)/$(MODULE_ELF)

$(LOADER_O): _loader.c
	$(GCC) $(MY_CFLAGS) -c $< -o $@

%.o: %.c
	$(GCC) $(MY_CFLAGS) -c $< -o $@

%.o: %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

# El -no-pie tiene que estar si o si porque el linker está tratando de generar un objeto
#  PIE (Position Independent Executable), pero nuestro código tiene referencias absolutas
# que no son compatibles

# Linkeo para binario (usa OUTPUT_FORMAT del linker script)
$(OUTPUT_DIR)/$(MODULE_BIN): $(LOADER_O) $(COBJECTS) $(ASMOBJS) \
                              ../../UserLib/libuserland.a \
                              ../PongisGolf/libpongisgolf.a
	$(GCC) $(MY_CFLAGS) -no-pie -T $(LDSCRIPT) \
          $(LOADER_O) $(COBJECTS) $(ASMOBJS) ../PongisGolf/libpongisgolf.a ../../UserLib/libuserland.a \
          -o $@ $(LDFLAGS)

# Linkeo para ELF (ignora OUTPUT_FORMAT del linker script con -Wl,--oformat)
$(OUTPUT_DIR)/$(MODULE_ELF): $(LOADER_O) $(COBJECTS) $(ASMOBJS) \
                              ../../UserLib/libuserland.a \
                              ../PongisGolf/libpongisgolf.a
	$(GCC) $(MY_CFLAGS) -no-pie -T $(LDSCRIPT) -Wl,--oformat=elf64-x86-64 \
          $(LOADER_O) $(COBJECTS) $(ASMOBJS) ../PongisGolf/libpongisgolf.a ../../UserLib/libuserland.a \
          -o $@ $(LDFLAGS)

clean:
	rm -f $(LOADER_O) $(COBJECTS) $(ASMOBJS) $(OUTPUT_DIR)/$(MODULE_ELF) $(OUTPUT_DIR)/$(MODULE_BIN)
