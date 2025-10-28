global context_switch
section .text
; void context_switch(uint64_t *prev_sp, uint64_t next_sp)
context_switch:
    mov [rdi], rsp
    mov rsp, rsi
    ret
