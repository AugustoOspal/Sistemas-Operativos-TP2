#pragma once
#include <stdint.h>

void switch_context(uint64_t *old_rsp_out, uint64_t new_rsp);
