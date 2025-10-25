include ../../Makefile.inc

MODULE     := 0000-shell.bin
LDSCRIPT   := shell.ld

MY_CFLAGS  := $(GCCFLAGS) \
               -Ilib \
               -I../../UserUtils/UserLib/include \
               -I../PongisGolf/lib

CSOURCES   := $(wildcard [^_]*.c)
COBJECTS   := $(CSOURCES:.c=.o)
ASMSOURCES := $(wildcard *.asm)
ASMOBJS    := $(ASMSOURCES:.asm:.o)
LOADER_O   := _loader.o

.PHONY: all clean

all: $(MODULE)

$(LOADER_O): _loader.c
	$(GCC) $(MY_CFLAGS) -c $< -o $@

%.o: %.c
	$(GCC) $(MY_CFLAGS) -c $< -o $@

%.o: %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(MODULE): $(LOADER_O) $(COBJECTS) $(ASMOBJS) \
            ../../UserUtils/UserLib/libuserland.a \
            ../PongisGolf/libpongisgolf.a
	$(GCC) $(MY_CFLAGS) -T $(LDSCRIPT) \
          $(LOADER_O) $(COBJECTS) $(ASMOBJS) ../PongisGolf/libpongisgolf.a ../../UserUtils/UserLib/libuserland.a \
          -o ../../$(MODULE) $(LDFLAGS)

clean:
	rm -f $(LOADER_O) $(COBJECTS) $(ASMOBJS) ../../$(MODULE)
