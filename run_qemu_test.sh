#!/bin/bash
cd /home/artem-kholev/Desktop/Eco.Lab4
echo "Starting QEMU with EcoOS kernel..."
env -i PATH=/usr/bin:/bin TERM=$TERM HOME=$HOME DISPLAY=$DISPLAY XAUTHORITY=$XAUTHORITY \
  /usr/bin/qemu-system-aarch64 -M raspi3b \
  -kernel MySimpleEcoOS/AssemblyFiles/EcoOS/aarch64_gcc_13_2_1/kernel8.img
