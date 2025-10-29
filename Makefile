all:
	cd x64BareBones && make all

clean:
	cd x64BareBones && make clean

format:
	clang-format -style=file -i $$(find x64BareBones/Kernel x64BareBones/Userland -name "*.c" -o -name "*.h")

test:
	cd x64BareBones && make test

check:
	cppcheck --quiet --enable=all --force --inconclusive x64BareBones/Kernel x64BareBones/Userland

.PHONY: all clean format test check