#!/bin/bash

./run.sh gdb &
QEMU_PID=$!

sleep 2
docker start Sistemas-Operativos-TP2
docker exec -it Sistemas-Operativos-TP2 bash -c "cd root && gdb"
docker stop Sistemas-Operativos-TP2

kill $QEMU_PID