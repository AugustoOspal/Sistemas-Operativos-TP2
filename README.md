# Trabajo Práctico 2 - Sistemas Operativos

## Integrantes
* Manuel Suñol - msunol@itba.edu.ar
* Mateo Stella - mastella@itba.edu.ar
* Augusto Felipe Ospal Madeo - aospalmadeo@itba.edu.ar

## Indice
- [Manual de Usuario](#manual-de-usuario)
  - [Requisitos](#requisitos)
  - [Preparar el entorno](#preparar-el-entorno)
  - [Compilación](#compilación)
  - [Ejecución](#ejecución)
  - [Debugger](#debugger)
- [Limitaciones](#limitaciones)
- [Citas](#citas)
- [Comandos y tests](#comandos-y-tests-disponibles)

## Manual de Usuario

### Requisitos
Es requisito tener instalados los siguientes programas:
* gcc
* qemu
* docker
* make
* nasm
* gdb (opcional)
---
### Preparar el entorno
Una vez ubicado en el directorio deseado, desde la terminal ejecutar los siguientes comandos:
```bash
docker pull agodio/itba-so-multi-platform:3.0
git clone https://github.com/AugustoOspal/Sistemas-Operativos-TP2.git
cd Sistemas-Operativos-TP2/x64BareBones
docker run -d -v ${PWD}:/root --security-opt seccomp:unconfined --add-host=host.docker.internal:host-gateway -it --name Sistemas-Operativos-TP2 agodio/itba-so-multi-platform:3.0
```
---
### Compilación
Dentro del directorio Sistemas-Operativos-TP2/x64BareBones, ejecutar:
```bash
./compile.sh
```
También se puede compilar el proyecto para que use el memory manager buddy de esta forma:
```bash
./compile.sh buddy
```
---
### Ejecución
Dentro del directorio Sistemas-Operativos-TP2/x64BareBones, ejecutar:
```bash
./run.sh
```
---
### Limpieza de archivos temporales
Dentro del directorio Sistemas-Operativos-TP2/x64BareBones, ejecutar:
```bash
./clean.sh
```
---
### Debugger
Antes de debugger se debe compilar el proyecto. Dentro del directorio Sistemas-Operativos-TP2/x64BareBones, ejecutar:

Para debugger localmente:
```bash
./run.sh gdb
```
Para debugger en docker:
```bash
./run.sh gdb docker
```
---
## Limitaciones
- No está implementado el cambio en las prioridades de los procesos de forma dinamica
- No se puede variar el quantum y todos los procesos tienen el mismo
- Si un proceso nunca muere y no espera a sus hijos, reaped no se limpia automaticamente y
  los hijos zombies quedan ocupando espacio
- IPC y sincronizacion están limitados a pipes y semaforos, y faltan mecanismos fundamentales como memoria compartida
- La terminal no permite al usuario scrollear
- Los pipes solo conectan hasta procesos como máximo
## Citas
Para este trabajo práctico utilizamos:

- Libros
  - Abraham-Silberschatz-Operating-System-Concepts-10th-2018
  - Andrew S. Tanenbaum - Operating Systems. Design and Implementation
  - Andrew S. Tanenbaum - Structured Computer Organization
  - Andrew-S.-Tanenbaum-Modern-Operating-Systems
  - Operating Systems Three Easy Pieces
  - The C programming language 2nd edition
  - The Linux Programming Interface
- Clases teoricas y prácticas
- Videos de YouTube
  - https://www.youtube.com/watch?v=70auqrv84y8&list=PLpe9fg9SgDEuUOArrb_yTHfsCLXTCuPgd&index=3
  - https://www.youtube.com/watch?v=3KJeK-UUADA&list=PLpe9fg9SgDEuUOArrb_yTHfsCLXTCuPgd&index=4

A lo largo del trabajo práctico comentamos en que partes usamos cada recurso de cada libro

También usamos inteligencia artificial (principalmente claude CLI), aunque no fue de mucha ayuda porque suele incorporar 
bugs muy dificiles de detectar al largo plazo, por lo que solamente lo usamos para verificar algunas implementaciones
---
## Comandos y tests disponibles
- help – muestra todos los comandos registrados. Sin parámetros.
- pongisgolf – lanza el juego Pongis Golf (queda en foreground, Ctrl+C para salir).
- zoom in / zoom out – cambia el tamaño del texto en pantalla; no aceptan argumentos.
- clear – limpia el framebuffer.
- date – imprime fecha y hora actual (usa RTC).
- registers – vuelca el snapshot tomado con Ctrl+R; sin argumentos.
- busywait – bucle CPU-bound en userland para probar Ctrl+R.
- busywaitkernel – duerme en kernel (sleepMilli) para probar Ctrl+R en contexto kernel.
- exit – finaliza la shell (apaga el userland).
- nice <pid> <prio> – cambia la prioridad (0–3) del proceso indicado.
- block <pid> – si el proceso está RUNNING/READY, lo bloquea; si ya está bloqueado, lo desbloquea (toggle).
- ps – lista todos los procesos con PID, prioridad, estado y dirección de pila.
- cat – eco interactivo de stdin (Ctrl+D para cerrar).
- mem – imprime estadísticas del allocator (total/used/free).
- loop – proceso que imprime su PID cada unos ticks. Sin argumentos.
- wc – cuenta líneas/palabras/caracteres de la entrada hasta Enter.
- filter – copia caracteres vocálicos de la entrada hasta Enter.
- kill <pid> – envía SIGTERM (killProcess) al PID destino.
- mvar <args…> – corre la app “mvar” con los parámetros dados (ver docs de la app).
- test_mm <max_bytes> – estresa el allocator pidiendo/liberando hasta max_bytes.
- test_priority <max> – crea procesos zero_to_max; <max> es el límite del contador interno.
- test_processes <n> – lanza n loops infinitos y los mata/bloquea aleatoriamente.
- test_synchro <iters> <use_sem> 0 – pares inc/dec sobre global; use_sem=1 usa semáforo.

## Sintaxis especial
- command1 | command2 arma un pipe unidireccional (stdout del primero a stdin del segundo).
- command & ejecuta en background (la shell no hace waitPid). También funciona con pipelines completos: cmd1 | cmd2 &.

## Atajos de teclado
- Ctrl+C – mata el proceso en foreground (usa getForegroundPid + killProcess).
- Ctrl+D – envía EOF al stdin actual; las lecturas posteriores devuelven 0 bytes y los programas suelen salir.
- Ctrl+R sigue disponible para capturar registros, aunque no detiene procesos.