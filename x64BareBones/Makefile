
all:  bootloader kernel userland image

buddy: bootloader kernel-buddy userland image

bootloader:
	cd Bootloader; make all

kernel:
	cd Kernel; make all

kernel-buddy:
	cd Kernel; make all MEMORY_MANAGER=buddy

userland:
	cd Userland; make all

image: kernel bootloader userland
	cd Image; make all

test:
	cd test; make test-kernel

clean:
	cd Bootloader; make clean
	cd Image; make clean
	cd Kernel; make clean
	cd Userland; make clean
	cd test; make clean

.PHONY: bootloader image collections kernel kernel-buddy userland all buddy clean test
