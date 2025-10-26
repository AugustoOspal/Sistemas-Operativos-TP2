include Makefile.inc

.PHONY: all clean UserLib UserApps shellDataModule

all: UserLib UserApps shellDataModule

# primero compilamos la librería de utilidades
UserLib:
	$(MAKE) -C UserLib

# luego compilamos las aplicaciones (Terminal, PongisGolf)
UserApps: UserLib
	$(MAKE) -C UserApps

shellDataModule:
	printf "Shell data section." > 0001-shellData.bin
	dd if=/dev/zero bs=1 count=1 >> 0001-shellData.bin

clean:
	$(MAKE) -C UserLib clean
	$(MAKE) -C UserApps clean
	rm -f 0001-shellData.bin 0000-shell.bin