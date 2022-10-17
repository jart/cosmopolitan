#!/bin/sh
make -j16 o//net/turfwar/blackholed.elf &&
sudo chown root o//net/turfwar/blackholed.elf &&
sudo chmod 06755 o//net/turfwar/blackholed.elf &&
exec o//net/turfwar/blackholed.elf
