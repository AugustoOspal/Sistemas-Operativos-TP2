#!/bin/bash
docker start Sistemas-Operativos-TP2
docker exec -it Sistemas-Operativos-TP2 make clean -C /root/Toolchain
docker exec -it Sistemas-Operativos-TP2 make clean -C /root/
docker stop Sistemas-Operativos-TP2