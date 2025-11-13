#include "keyboardDriver.h"
#include "../../proc/include/process.h"
#include "../../sched/include/scheduler.h"
#include "../../semaphore/include/semaphore.h"

// --- Definiciones de Scancodes (Set 1) ---
// Teclas Modificadoras (Make codes)
#define SC_LSHIFT_PRESS 0x2A
#define SC_RSHIFT_PRESS 0x36
#define SC_CAPSLOCK_PRESS 0x3A
#define SC_CTRL_PRESS 0x1D
#define SC_ALT_PRESS 0x38

// Teclas Modificadoras (Break codes - make_code | 0x80)
#define SC_LSHIFT_RELEASE (SC_LSHIFT_PRESS | 0x80)
#define SC_RSHIFT_RELEASE (SC_RSHIFT_PRESS | 0x80)
#define SC_CTRL_RELEASE (SC_CTRL_PRESS | 0x80)
#define SC_ALT_RELEASE (SC_ALT_PRESS | 0x80)

// Estructura para el estado de las teclas Modificadoras
typedef struct
{
	uint8_t lshift : 1;
	uint8_t rshift : 1;
	uint8_t caps_lock_on : 1;
	uint8_t ctrl : 1;
	uint8_t alt : 1;
} ModifierState_t;

// Inicializar todos los flags a 0
static ModifierState_t kbd_modifier_state = {0};

// Mapa de Scancodes a Caracteres (US QWERTY Layout)
// Caracteres sin modificadores (o con CapsLock para no-letras)
static const char scancode_to_ascii_map_normal[128] = {
	0,	 0x1B, '1',	 '2',  '3', '4',  '5',	'6', '7',
	'8', '9',  '0',	 '-',  '=', '\b', '\t', // 0x00-0x0F (ESC, 1-0, -, =, Backspace, Tab)
	'q', 'w',  'e',	 'r',  't', 'y',  'u',	'i', 'o',
	'p', '[',  ']',	 '\n', 0,	'a',  's', // 0x10-0x1F (q-p, [, ], Enter, LCtrl, a, s)
	'd', 'f',  'g',	 'h',  'j', 'k',  'l',	';', '\'',
	'`', 0,	   '\\', 'z',  'x', 'c',  'v', // 0x20-0x2F (d-l, ;, ', `, LShift, \, z, x, c, v)
	'b', 'n',  'm',	 ',',  '.', '/',  0,	'*', 0,
	' ', 0,	   0,	 0,	   0,	0,	  0, // 0x30-0x3F (b-m, ,, ., /, RShift, Keypad *, LAlt, Space, CapsLock, F1-F10...)
};

// Caracteres con Shift presionado (o con CapsLock para letras si Shift no está presionado)
static const char scancode_to_ascii_map_shifted[128] = {
	0,	 0x1B, '!', '@', '#', '$', '%',	 '^', '&', '*', '(', ')', '_', '+', '\b', '\t', 'Q', 'W', 'E', 'R', 'T', 'Y',
	'U', 'I',  'O', 'P', '{', '}', '\n', 0,	  'A', 'S', 'D', 'F', 'G', 'H', 'J',  'K',	'L', ':', '"', '~', 0,	 '|',
	'Z', 'X',  'C', 'V', 'B', 'N', 'M',	 '<', '>', '?', 0,	 '*', 0,   ' ', 0,	  0,	0,	 0,	  0,   0,
	// ...
};

#define KEYBOARD_BUFFER_SIZE 256
static char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static unsigned int buffer_write_idx = 0;
static unsigned int buffer_read_idx = 0;
static unsigned int buffer_count = 0;

// Flag para indicar EOF (Ctrl+D)
static bool kbd_eof_flag = false;

void keyboard_init()
{
	semOpen("keyboard_sem", 0);
}

bool kbd_check_eof(void)
{
	if (kbd_eof_flag)
	{
		kbd_eof_flag = false;
		return true;
	}
	return false;
}

char procesScanCode(const unsigned int scancode)
{
	if (scancode == 0)
	{
		return -1;
	}

	/*
		Si el scancode tiene seteado el septimo bit mas significativo
		significa que la tecla fue liberada, sino que fue apretada
	*/
	const int is_press = !(scancode & 0x80);

	// El scancode base sin el bit de apretado/liberacion
	unsigned int make_code = scancode & 0x7F;

	// 1. Actualizar estado de teclas modificadoras
	switch (make_code)
	{
		case SC_LSHIFT_PRESS:
			kbd_modifier_state.lshift = is_press;
			return;
		case SC_RSHIFT_PRESS:
			kbd_modifier_state.rshift = is_press;
			return;
		case SC_CAPSLOCK_PRESS:
			if (is_press)
			{ // CapsLock es un toggle, actúa solo al apretar
				kbd_modifier_state.caps_lock_on = !kbd_modifier_state.caps_lock_on;
			}
			return;
		case SC_CTRL_PRESS:
			kbd_modifier_state.ctrl = is_press;
			return;
		case SC_ALT_PRESS:
			kbd_modifier_state.alt = is_press;
			return;
	}

	// 2. Si es un "press" de una tecla no modificadora, procesar para agarrar el carácter
	char final_char = 0;
	if (is_press)
	{
		char char_normal = 0;
		char char_shifted = 0;

		if (make_code < 128)
		{
			char_normal = scancode_to_ascii_map_normal[make_code];
			char_shifted = scancode_to_ascii_map_shifted[make_code];
		}

		// Si es no imprimible o no esta mapeada
		if (char_normal == 0)
		{
			return;
		}

		int is_alpha_lower = (char_normal >= 'a' && char_normal <= 'z');
		int any_shift_pressed = kbd_modifier_state.lshift || kbd_modifier_state.rshift;

		// Esto esta asi en vez de un final_char ? mas simple para el manejo del Caps
		if (any_shift_pressed)
		{
			final_char = char_shifted;
			if (kbd_modifier_state.caps_lock_on && is_alpha_lower)
			{
				if (char_shifted >= 'A' && char_shifted <= 'Z' && (char_shifted - 'A' == char_normal - 'a'))
				{
					final_char = char_normal;
				}
			}
		}
		else
		{
			final_char = char_normal;
			if (kbd_modifier_state.caps_lock_on && is_alpha_lower)
			{
				final_char = char_shifted;
			}
		}
	}
	return final_char;
}

void loadCharToBuffer(char c)
{
	if (c != 0 && buffer_count < KEYBOARD_BUFFER_SIZE)
	{
		keyboard_buffer[buffer_write_idx] = c;
		buffer_write_idx = (buffer_write_idx + 1) % KEYBOARD_BUFFER_SIZE;
		buffer_count++;
	}
}

void keyboard_handler(Registers_t *regs)
{
	uint8_t scanCode = getKeyCode();
	char c = procesScanCode(scanCode);

	// Ctrl+R: Captura snapshot de registros
	if (kbd_modifier_state.ctrl && (c == 'r' || c == 'R'))
	{
		loadSnapshot(regs);
	}
	// Ctrl+C: Terminar proceso en foreground
	else if (kbd_modifier_state.ctrl && (c == 'c' || c == 'C'))
	{
		const uint64_t fgPid = getForegroundPid();
		if (fgPid != 0)
		{
			killProcess(fgPid);
		}
	}
	// Ctrl+D: Enviar EOF
	else if (kbd_modifier_state.ctrl && (c == 'd' || c == 'D'))
	{
		kbd_eof_flag = true;
		semPost(getSem("keyboard_sem")); // Despertar proceso esperando lectura
	}
	else if (c != 0)
	{
		loadCharToBuffer(c);
		semPost(getSem("keyboard_sem")); // Señalizar que hay una tecla disponible
	}
}

char kbd_get_char()
{
	semWait(getSem("keyboard_sem")); // Esperar hasta que haya una tecla disponible
	if (buffer_count == 0)
		return 0; // Buffer vacío

	char c = keyboard_buffer[buffer_read_idx];
	buffer_read_idx = (buffer_read_idx + 1) % KEYBOARD_BUFFER_SIZE;
	buffer_count--;

	return c;
}