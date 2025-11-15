// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "stringLib.h"

uint8_t strlen(const char *str)
{
	uint8_t len = 0;
	while (*str++)
	{
		len++;
	}
	return len;
}

uint8_t strcmp(const char *s1, const char *s2)
{
	while (*s1 != '\0' && *s2 != '\0')
	{
		if (*s1 != *s2)
			return *s1 - *s2;
		s1++;
		s2++;
	}
	return *s1 - *s2;
}

int atoi(const char *str)
{
	int result = 0;
	int sign = 1;

	while (*str == ' ')
	{
		str++;
	}

	if (*str == '-')
	{
		sign = -1;
		str++;
	}
	else if (*str == '+')
	{
		str++;
	}

	while (*str >= '0' && *str <= '9')
	{
		result = result * 10 + (*str - '0');
		str++;
	}

	return sign * result;
}

void itoa(int num, char *str, const int base)
{
	int i = 0;
	int isNegative = 0;

	if (num == 0)
	{
		str[i++] = '0';
		str[i] = '\0';
		return;
	}

	if (num < 0 && base == 10)
	{
		isNegative = 1;
		num = -num;
	}

	while (num != 0)
	{
		int rem = num % base;
		str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
		num = num / base;
	}

	if (isNegative)
	{
		str[i++] = '-';
	}

	str[i] = '\0';

	// Invertir el string
	int start = 0;
	int end = i - 1;
	while (start < end)
	{
		char temp = str[start];
		str[start] = str[end];
		str[end] = temp;
		start++;
		end--;
	}
}
