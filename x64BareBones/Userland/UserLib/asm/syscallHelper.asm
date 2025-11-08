GLOBAL sys_clearScreen
GLOBAL sys_putPixel
GLOBAL sys_drawChar
GLOBAL sys_drawString
GLOBAL sys_drawRectangle
GLOBAL sys_drawCircle
GLOBAL sys_drawDecimal
GLOBAL sys_drawHexa
GLOBAL sys_drawBin
GLOBAL sys_getScreenWidth
GLOBAL sys_getScreenHeight

GLOBAL sys_kbdGetChar

GLOBAL sys_write
GLOBAL sys_read

GLOBAL sys_getRegisters
GLOBAL sys_getDateTime

GLOBAL sys_zoomIn
GLOBAL sys_zoomOut

GLOBAL sys_playSound
GLOBAL sys_sleepMilli

GLOBAL sys_createProcess
GLOBAL sys_deleteProcess
GLOBAL sys_getPid
GLOBAL sys_getAllProcessesInfo
GLOBAL sys_changeProcessPriority
GLOBAL sys_blockProcess
GLOBAL sys_unblockProcess
GLOBAL sys_yield

GLOBAL sys_semOpen
GLOBAL sys_semClose
GLOBAL sys_semWait
GLOBAL sys_semPost
GLOBAL sys_semTryWait
GLOBAL sys_semGetValue
GLOBAL sys_semUnlink

GLOBAL opCodeException

section .text

%macro syscall 1
    mov rax, %1
    int 80h
    ret
%endmacro

sys_write:              syscall 0x1
sys_read:               syscall 0x2

sys_getRegisters:       syscall 0x04
sys_getDateTime:        syscall 0x05

sys_zoomIn:             syscall 0x06
sys_zoomOut:            syscall 0x07

; Video
sys_clearScreen:        syscall 0x10
sys_putPixel:           syscall 0x11
sys_drawChar:           syscall 0x12
sys_drawString:         syscall 0x13
sys_drawRectangle:      syscall 0x14
sys_drawDecimal:        syscall 0x15
sys_drawHexa:           syscall 0x16
sys_drawBin:            syscall 0x17
sys_getScreenWidth:     syscall 0x18
sys_getScreenHeight:    syscall 0x19
sys_drawCircle:         syscall 0x21    ;TODO: Organizar esto

; Teclado
sys_kbdGetChar:         syscall 0x20

; Sonido
sys_playSound:          syscall 0x30

; Tiempo
sys_sleepMilli:          syscall 0x40

; Procesos
sys_createProcess:          syscall 0x50
sys_deleteProcess:          syscall 0x51
sys_getPid:                 syscall 0x52
sys_getAllProcessesInfo:    syscall 0x53
sys_changeProcessPriority:  syscall 0x55
sys_blockProcess:           syscall 0x56
sys_unblockProcess:         syscall 0x57
sys_yield:                  syscall 0x58

; Semaforos
sys_semOpen:                syscall 0x60
sys_semClose:               syscall 0x61
sys_semWait:                syscall 0x62
sys_semPost:                syscall 0x63
sys_semTryWait:             syscall 0x64
sys_semGetValue:            syscall 0x65
sys_semUnlink:              syscall 0x66

; Ecceptions
opCodeException:
	ud2
	ret