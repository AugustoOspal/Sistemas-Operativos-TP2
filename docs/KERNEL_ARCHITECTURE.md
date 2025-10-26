# Arquitectura del Kernel y Plan de Crecimiento

## Descripción General

Este documento describe la arquitectura modular del kernel x64BareBones, diseñada para escalar desde un SO bare-metal básico hasta un kernel completamente funcional con manejo de memoria, planificación de procesos y mecanismos IPC.

## Arquitectura Actual

### Modelo de Capas

| Capa | Descripción | Directorio |
|------|-------------|-----------|
| **Llamadas al Sistema** | Interfaz usuario-kernel | `syscall/` |
| **Servicios Principales** | Planificador, Memoria, IPC, FS | `mm/`, `sched/`, `ipc/`, `fs/` |
| **Manejo de Interrupciones** | Excepciones e IRQ | `interrupt/` |
| **Controladores de Dispositivos** | Abstracción de hardware | `drivers/` |
| **Utilidades** | Funciones internas del kernel | `lib/` |
| **Código de Arquitectura** | Específico de x86-64 | `arch/x86_64/` |

## Estructura de Directorios
La idea al ir avanzando con el proyecto es que mantenga esta estructura o similar:

```
Kernel/
│
├── arch/x86_64/                 # Código específico de arquitectura
│   ├── include/
│   │   ├── defs.h               - Definiciones de CPU, layout de memoria
│   │   ├── registers.h          - Estructuras de registros
│   │   └── interrupts.h         - Vectores de interrupciones
│   ├── asm/
│   │   ├── loader.asm           - Punto de entrada del bootloader
│   │   ├── interrupts.asm       - Manejadores de excepciones/IRQ
│   │   └── context_switch.asm   - Cambio de contexto de procesos (futuro)
│   └── boot/
│       └── kernel.ld            - Script del linker
│
├── core/                        # Núcleo del kernel
│   ├── include/
│   │   ├── kernel.h             - Declaraciones del kernel
│   │   └── moduleLoader.h       - Interfaz de carga de módulos
│   ├── kernel.c                 - Punto de entrada main() del kernel
│   └── moduleLoader.c           - Cargador de módulos de userland
│
├── interrupt/                   # Manejo de interrupciones y excepciones
│   ├── include/
│   │   ├── idt.h                - Declaraciones de IDT
│   │   ├── exceptions.h         - Manejadores de excepciones
│   │   ├── irq.h                - Dispatch de IRQ
│   │   └── interrupts.h         - Utilidades de interrupciones
│   ├── idt.c                    - Configuración de IDT
│   ├── exceptions.c             - Implementaciones de manejadores
│   ├── irq.c                    - Dispatcher de IRQ
│   └── asm/
│       └── interrupts.asm       - Manejadores de bajo nivel
│
├── drivers/                     # Controladores de dispositivos
│   ├── include/
│   │   ├── video.h              - Interfaz del controlador de video
│   │   ├── keyboard.h           - Interfaz del controlador de teclado
│   │   ├── sound.h              - Interfaz del controlador de sonido
│   │   ├── timer.h              - Interfaz de timer/RTC
│   │   └── device.h             - Interfaz genérica de dispositivo
│   ├── video/
│   │   ├── videoDriver.c        - Controlador de framebuffer VBE
│   │   ├── fonts.c              - Datos de fuentes y renderizado
│   │   └── fonts.h              - Definiciones de fuentes
│   ├── keyboard/
│   │   └── keyboardDriver.c     - Controlador PS/2
│   ├── sound/
│   │   └── soundDriver.c        - Sonido por speaker de PC
│   └── timer/
│       └── timerDriver.c        - Funciones de RTC y timer
│
├── syscall/                     # Interfaz de llamadas al sistema
│   ├── include/
│   │   └── syscalls.h           - Declaraciones de syscalls
│   └── syscalls.c               - Dispatcher e implementación de syscalls
│
├── lib/                         # Utilidades del kernel (NO subsistemas)
│   ├── include/
│   │   ├── lib.h                - Declaraciones de librería
│   │   ├── defs.h               - Definiciones comunes
│   │   └── console.h            - Salida a consola
│   ├── lib.c                    - memset, memcpy, etc.
│   ├── console/
│   │   ├── naiveConsole.c       - Consola de debugging temprana
│   │   └── naiveConsole.h       - Interfaz de consola
│   └── string/
│       └── string.c             - Utilidades de strings (futuro)
│
├── mm/                          # Manejo de memoria (FUTURO)
│   ├── include/
│   │   ├── mm.h                 - Interfaz del gestor de memoria
│   │   ├── paging.h             - Estructuras de paging
│   │   ├── malloc.h             - Asignación de memoria
│   │   └── vm.h                 - Memoria virtual
│   ├── paging.c                 - Setup y gestión de tablas de páginas
│   ├── malloc.c                 - Asignador de memoria del kernel
│   └── vm.c                     - Gestión de memoria virtual
│
├── sched/                       # Planificación de procesos (FUTURO)
│   ├── include/
│   │   ├── sched.h              - Interfaz del planificador
│   │   ├── process.h            - Estructuras de procesos/threads
│   │   └── context.h            - Cambio de contexto
│   ├── scheduler.c              - Algoritmo de planificación
│   ├── process.c                - Gestión de procesos
│   └── asm/
│       └── context_switch.asm   - Implementación de cambio de contexto
│
├── fs/                          # Sistema de archivos (FUTURO)
│   ├── include/
│   │   ├── fs.h                 - Interfaz del sistema de archivos
│   │   ├── vfs.h                - Sistema de archivos virtual
│   │   └── inode.h              - Estructuras de inodes
│   ├── vfs.c                    - Abstracción VFS
│   ├── inode.c                  - Gestión de inodes
│   └── drivers/
│       └── bmfs.c               - Controlador BMFS
│
├── ipc/                         # Comunicación inter-procesos (FUTURO)
│   ├── include/
│   │   ├── ipc.h                - Interfaz de IPC
│   │   ├── signal.h             - Manejo de señales
│   │   └── pipe.h               - Estructuras de pipes
│   ├── signal.c                 - Implementación de señales
│   ├── pipe.c                   - Comunicación por pipes
│   └── semaphore.c              - Primitivas de sincronización (futuro)
│
├── include/                     # Headers globales del kernel
│   ├── types.h                  - Definiciones de tipos comunes
│   ├── kernel.h                 - Declaraciones del kernel
│   └── config.h                 - Constantes de configuración
│
└── Makefile                     # Configuración de compilación
```

## Descripción de Componentes

### arch/x86_64/
Código específico de plataforma aislado de la lógica central del kernel.
- Definiciones de registros específicas de CPU
- Layout de memoria (kernel en 0x100000, etc.)
- Punto de entrada en assembly y manejadores de interrupciones
- Script del linker

Esta estructura permite portar el kernel a otras arquitecturas (ARM, RISC-V) implementando solo esta capa.

### core/
Inicialización del kernel y carga de módulos.
- `kernel.c:main()` - Punto de entrada del kernel
- `moduleLoader.c` - Carga módulos de userland desde payload BMFS

### interrupt/
Manejo de interrupciones y excepciones de CPU.
- Setup de IDT (Interrupt Descriptor Table)
- Manejadores de excepciones (#DE, #UD, etc.)
- Routing de IRQ para interrupciones de hardware
- Incluye manejadores en ensamblador para operaciones de bajo nivel

### drivers/
Capa de abstracción de hardware.
- Encapsula interacción con hardware
- Proporciona interfaces limpias para el kernel
- Puede reemplazarse sin afectar el kernel core

**Controladores actuales:**
- Video: Acceso directo a framebuffer VBE
- Teclado: Escaneo de teclado PS/2
- Sonido: PC speaker via PIT
- Timer: Acceso a RTC

### syscall/
Interfaz usuario-kernel.
- Dispatcher de syscalls (manejador int 0x80)
- Valida solicitudes del usuario
- Llama a funciones del kernel apropiadas

### lib/
Utilidades solo para el kernel (NO exportadas a userland).
- `memset()`, `memcpy()` - Operaciones de memoria
- `naiveConsole` - Consola temprana para debugging
- Utilidades de strings

**Importante:** Estos son internos del kernel. Userland tiene su propio UserLib con wrappers de syscalls.

---

## Compilación

### Estructura del Makefile Actual
```
Kernel/Makefile:
  - Compila todos los .c en Kernel/
  - Incluye todos los headers en Kernel/include/
  - Linkea con kernel.ld

Estructura del Makefile Futuro:
  - Makefile top-level orquesta compilación
  - Cada subsistema tiene su propio Makefile (opcional)
  - Evita recompilar subsistemas sin cambios
```

### Banderas de Compilación
```
KERNEL_CFLAGS = -m64 -nostdlib -ffreestanding ...
- Bare-metal, sin librería estándar
- Target x86-64 de 64 bits
- Sin código independiente de posición (kernel carga en dirección fija)
```

## Organización de Headers

### Convenciones de Includes
```
Dentro de Kernel/:
#include "arch/x86_64/defs.h"         - Definiciones de arquitectura
#include "interrupt/idt.h"             - Del subsistema interrupt/
#include "drivers/video.h"             - De drivers/

Desde Userland:
#include "syscallLib.h"                - Wrappers de syscalls
#include "videoLib.h"                  - Abstracción de dispositivo
```
