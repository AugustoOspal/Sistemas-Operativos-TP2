; arch/x86_64/start_process.asm
global start_process

; void start_process(uint64_t saved_rsp);
; rdi = saved_rsp (puntero al inicio del TrapFrame del proceso)
start_process:
    mov     rsp, rdi

    ; Coincidir con tu popState (orden exacto):
    pop     r15
    pop     r14
    pop     r13
    pop     r12
    pop     r11
    pop     r10
    pop     r9
    pop     r8
    pop     rsi
    pop     rdi
    pop     rbp
    pop     rdx
    pop     rcx
    pop     rbx
    pop     rax

    ; Ahora el tope de la pila es RIP, luego CS, luego RFLAGS.
    pop     rax           ; rax = RIP (entry_point)
    add     rsp, 16       ; descartar CS y RFLAGS (no los usamos en esta prueba)
    jmp     rax    