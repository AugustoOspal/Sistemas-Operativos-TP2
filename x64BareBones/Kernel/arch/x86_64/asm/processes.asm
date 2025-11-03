GLOBAL initializeProcess

section .rodata

section .text

; No podemos usar la macro de pushState que esta en interrupts.asm
; porque no se pueden exportar macros entre archivos asm.
%macro pushState 0
	push rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro



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
    ret

section .bss