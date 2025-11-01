GLOBAL initializeProcess
EXTERN pushState

section .rodata

section .text



initializeProcess:
    mov r9, rsp             ; Armo stackframe
    mov r10, rbp

    mov rbp, rsi            ; Estos dos son para que todos los push
    mov rsp, rsi            ; queden en el stack del nuevo proceso

    push 0                  ; SS
    push rsi                ; RSP (stackEnd)
    push 0x202              ; RFLAGS. Como es nuevo y no tiene ninguna operacion
                            ; aritmetica previa, arranca en 0x202 (lo dice en la clase)

    push 0x8                ; CS
    push rdi                ; RIP (direccion de startWrapper)

    mov rdi, rdx            ; arg0 de startWrapper (main)
    mov rsi, rcx            ; arg1 de startWrapper (argc)
    mov rdx, r8             ; arg2 de startWrapper (argv)

    pushState               ; Carga en el stack todos los registros, para que cuando
                            ; corra esto el sheduler los cargue y se los pase al wrapper

    mov rax, rsp            ; Devolvemos el puntero a la direccion para que
                            ; el scheduler pueda correrlo cargando el stack del proceso

    mov rsp, r9             ; Desarmo stackframe
    mov rbp, r10


section .bss