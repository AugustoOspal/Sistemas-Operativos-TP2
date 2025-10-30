// arch/x86_64/include/cpu_helpers.h
#pragma once
#include <stdint.h>

uint16_t read_cs(void);
uint64_t read_rflags(void);
