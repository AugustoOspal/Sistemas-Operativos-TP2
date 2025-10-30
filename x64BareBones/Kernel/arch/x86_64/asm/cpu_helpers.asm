global read_cs
global read_rflags

read_cs:
    ; Convención SysV: devolver en RAX
    xor     eax, eax
    mov     ax, cs     ; CS es de 16 bits; se extiende cero en RAX
    ret

read_rflags:
    pushfq
    pop     rax        ; RFLAGS completo en RAX
    ret
