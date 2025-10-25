include Makefile.inc

.PHONY: all clean UserUtils UserApps shellDataModule

all: UserUtils UserApps shellDataModule

# primero compilamos las utilidades (librerías)
UserUtils:
	$(MAKE) -C UserUtils

# luego compilamos las aplicaciones (Terminal, PongisGolf)
UserApps: UserUtils
	$(MAKE) -C UserApps

shellDataModule:
	printf "Shell data section." > 0001-shellData.bin
	dd if=/dev/zero bs=1 count=1 >> 0001-shellData.bin

clean:
	$(MAKE) -C UserUtils clean
	$(MAKE) -C UserApps clean
	rm -f 0001-shellData.bin 0000-shell.bin