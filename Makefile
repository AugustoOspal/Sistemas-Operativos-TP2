all:
	cd x64BareBones && make all

clean:
	rm ./PVS-Studio.log
	rm ./report.tasks
	rm ./strace_out
	cd x64BareBones && make clean

format:
	clang-format -style=file -i $$(find x64BareBones/Kernel x64BareBones/Userland -name "*.c" -o -name "*.h")

test:
	cd x64BareBones && make test

check: check-cppcheck check-pvs-studio

check-cppcheck:
	cppcheck --quiet --enable=all --force --inconclusive --suppress=comparePointers --suppress=zerodiv --suppress=unusedFunction --suppress=unusedVariable --suppress=unreadVariable --suppress=unsignedLessThanZero --suppress=constParameterPointer --suppress=constVariablePointer --suppress=unusedStructMember --suppress=variableScope --suppress=missingInclude --suppress=ctunullpointer x64BareBones/Kernel x64BareBones/Userland

check-pvs-studio:
	x64BareBones/compile.sh
	pvs-studio-analyzer credentials "PVS-Studio Free" "FREE-FREE-FREE-FREE"
	pvs-studio-analyzer trace -- make all
	pvs-studio-analyzer analyze
	plog-converter -a '64:1,2,3;GA:1,2,3;OP:1,2,3' -t tasklist -o report.tasks PVS-Studio.log

.PHONY: all clean format test check check-cppcheck check-pvs-studio