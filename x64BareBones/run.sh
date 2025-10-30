#!/bin/bash
# La parte a partir del -audiodev es para el sonido
#
# Para macOS: -audiodev coreaudio,id=speaker
# Para Linux: -audiodev pa,id=speaker -machine pcspk-audiodev=speaker

# Detectar el sistema operativo
OS="$(uname -s)"

if [[ "$OS" == Linux* ]]; then
    AUDIO_FLAGS="-audiodev pa,id=speaker"
elif [[ "$OS" == Darwin* ]]; then
    AUDIO_FLAGS="-audiodev coreaudio,id=speaker"
else
    echo "Error: Sistema operativo no soportado: $OS"
    echo "Este script solo funciona en Linux y macOS"
    exit 1
fi

if [[ "$1" == "gdb" ]]; then
    qemu-system-x86_64 -s -S -hda Image/x64BareBonesImage.qcow2 -m 512 -d int $AUDIO_FLAGS -machine pcspk-audiodev=speaker > /dev/null 2>&1 &
    QEMU_PID=$!
    docker start Sistemas-Operativos-TP2
    docker exec -it Sistemas-Operativos-TP2 bash -c "cd root && gdb"
    docker stop Sistemas-Operativos-TP2
    kill $QEMU_PID
    else
    qemu-system-x86_64 -hda Image/x64BareBonesImage.qcow2 -m 512 $AUDIO_FLAGS -machine pcspk-audiodev=speaker
fi
