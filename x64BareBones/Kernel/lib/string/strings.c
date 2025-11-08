#include "strings.h"

#include "../../mem/include/pmem.h"

char *strCpy(char *dest, const char *src)
{
	char *original = dest;
	while (*src)
	{
		*dest++ = *src++;
	}
	*dest = '\0';
	return original;
}

size_t strLen(const char *str)
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

char *strDup(const char *src)
{
	if (!src)
		return NULL;

	const int len = strLen(src);
	char *dup = mem_alloc(len + 1);
	if (!dup)
		return NULL;

	strCpy(dup, src);
	return dup;
}

static void write_unsigned(char **buf, unsigned int u)
{
	if (u >= 10)
		write_unsigned(buf, u / 10);
	*(*buf)++ = '0' + (u % 10);
}

static void write_int(char **buf, int d)
{
	if (d < 0)
	{
		*(*buf)++ = '-';
		d = -d;
	}
	write_unsigned(buf, (unsigned) d);
}

static void write_hex(char **buf, unsigned int x)
{
	char temp[8];
	int i = 0;
	if (x == 0)
	{
		*(*buf)++ = '0';
		return;
	}
	while (x)
	{
		int d = x & 0xF;
		temp[i++] = d < 10 ? '0' + d : 'a' + (d - 10);
		x >>= 4;
	}
	while (i--)
		*(*buf)++ = temp[i];
}

static void write_ulong(char **buf, unsigned long u)
{
	if (u >= 10)
		write_ulong(buf, u / 10);
	*(*buf)++ = '0' + (u % 10);
}

static void write_hex_long(char **buf, unsigned long x)
{
	char temp[16];
	int i = 0;
	if (x == 0)
	{
		*(*buf)++ = '0';
		return;
	}
	while (x)
	{
		int d = x & 0xF;
		temp[i++] = d < 10 ? '0' + d : 'a' + (d - 10);
		x >>= 4;
	}
	while (i--)
		*(*buf)++ = temp[i];
}

int ksprintf(char *str, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	char *buf = str;

	for (const char *p = format; *p; p++)
	{
		if (*p != '%')
		{
			*buf++ = *p;
			continue;
		}
		switch (*++p)
		{
			case 'd':
				write_int(&buf, va_arg(args, int));
				break;
			case 'u':
				write_unsigned(&buf, va_arg(args, unsigned));
				break;
			case 'x':
				write_hex(&buf, va_arg(args, unsigned));
				break;
			case 'l':
				if (*(p + 1) == 'u')
				{
					p++;
					write_ulong(&buf, va_arg(args, unsigned long));
				}
				else if (*(p + 1) == 'x')
				{
					p++;
					write_hex_long(&buf, va_arg(args, unsigned long));
				}
				break;
			case 'c':
				*buf++ = (char) va_arg(args, int);
				break;
			case 's':
			{
				char *s = va_arg(args, char *);
				if (s)
				{
					while (*s)
						*buf++ = *s++;
				}
				else
				{
					char *null_str = "(null)";
					while (*null_str)
						*buf++ = *null_str++;
				}
				break;
			}
			case '%':
				*buf++ = '%';
				break;
			default:
				*buf++ = '%';
				*buf++ = *p;
				break;
		}
	}
	*buf = '\0';
	va_end(args);
	return buf - str;
}