; arch/x86_64/switch_context.asm
global switch_context

; void switch_context(uint64_t *old_rsp_out, uint64_t new_rsp);
; rdi = &old_rsp_out, rsi = new_rsp

switch_context:
    mov [rdi], rsp    ; guarda el RSP actual en *old_rsp_out
    mov rsp, rsi      ; carga el nuevo RSP (del proceso siguiente)
    ret               ; retorna (usa la pila del nuevo proceso)
