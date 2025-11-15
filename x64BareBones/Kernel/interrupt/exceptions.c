// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "exceptions.h"
#include "fonts.h"
#include "syscalls.h"

#define CANT_REGS 20

// Nombres de los registros para la impresión (deben coincidir con Registers_t)
static const char *regs_strings[] = {"R15", "R14", "R13", "R12", "R11", "R10", "R9 ", "R8 ", "RSI", "RDI",
									 "RBP", "RDX", "RCX", "RBX", "RAX", "RIP", "CS ", "FLG", "RSP", "SS "};

static const char *exception_messages[] = {"Exception: Division by Zero", "Exception: Invalid Opcode"};

static void printRegisters(const Registers_t *regs);

void exceptionDispatcher(const Registers_t *regs, int exception)
{
	clearScreen();
	// Imprimir mensaje de la excepcion
	if (exception == ZERO_EXCEPTION_ID)
	{
		drawString(exception_messages[0], RED, 0, 0);
	}
	else if (exception == INVALID_OPERATION_CODE_ID)
	{
		drawString(exception_messages[1], RED, 0, 0);
	}

	printRegisters(regs);

	sleep(10000);
	resetCursorCoord();
	clearScreen();
}

static void printRegisters(const Registers_t *regs)
{
	const uint64_t *regs_array = (const uint64_t *) regs;
	unsigned int linesPrinted = 2; // Empezar a imprimir debajo del mensaje de excepcion

	for (int i = 0; i < CANT_REGS; i++)
	{
		unsigned int currentY = (getCurrentFontHeight() + FONT_CHAR_GAP) * linesPrinted++;
		drawString(regs_strings[i], RED, 0, currentY);
		drawHexa(regs_array[i], RED, 10 * (getCurrentFontWidth() + FONT_CHAR_GAP), currentY);
	}
}
