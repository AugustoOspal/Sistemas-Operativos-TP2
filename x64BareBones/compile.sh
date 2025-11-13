#!/bin/bash

# Check if buddy argument is provided
if [ "$1" = "buddy" ]; then
    MAKE_TARGET="buddy"
else
    MAKE_TARGET="all"
fi

docker start Sistemas-Operativos-TP2
docker exec -it Sistemas-Operativos-TP2 make clean -C /root/Toolchain
docker exec -it Sistemas-Operativos-TP2 make clean -C /root/
docker exec -it Sistemas-Operativos-TP2 make -C /root/Toolchain
docker exec -it Sistemas-Operativos-TP2 make $MAKE_TARGET -C /root/
docker stop Sistemas-Operativos-TP2
