# Trabajo Práctico 2 - Sistemas Operativos

## Integrantes
* Manuel Suñol - msunol@itba.edu.ar
* Mateo Stella - mastella@itba.edu.ar
* Augusto Felipe Ospal Madeo - aospalmadeo@itba.edu.ar

## Manual de Usuario
Es requisito tener instalados los siguientes programas:
* gcc
* qemu
* docker
* make
* nasm

### Preparar el entorno
Una vez ubicado en el directorio deseado, desde la terminal ejecutar los siguientes comandos:

```bash
docker pull agodio/itba-so-multi-platform:3.0
git clone https://github.com/AugustoOspal/Sistemas-Operativos-TP2.git
cd Sistemas-Operativos-TP2/x64BareBones
docker run -d -v ${PWD}:/root --security-opt seccomp:unconfined -it --name Sistemas-Operativos-TP2 agodio/itba-so-multi-platform:3.0
```

### Compilación
Dentro del directorio Arqui-TPE/x64BareBones, ejecutar:
```bash
./compile.sh
```

### Ejecución
Dentro del directorio Arqui-TPE/x64BareBones, ejecutar:
```bash
./run.sh
```

### Limpieza de archivos temporales
Dentro del directorio Arqui-TPE/x64BareBones, ejecutar:
```bash
./clean.sh
```