#!/bin/bash
# Xingcheng Jiang, Walt Panfil, and Maxim Zaman
# Team 1J
# Zero the floppy and add the bootloader
dd if=/dev/zero of=floppya.img bs=512 count=2880
nasm bootload.asm
dd if=bootload of=floppya.img bs=512 count=1 conv=notrunc

# Compile the kernel we write and add to floppy
bcc -ansi -c -o kernel.o kernel.c
as86 kernel.asm -o kernel_asm.o
ld86 -o kernel -d kernel.o kernel_asm.o
bcc -ansi -c -o shell.o shell.c
as86 lib.asm -o lib.o
ld86 -o shell -d shell.o lib.o
dd if=kernel of=floppya.img bs=512 conv=notrunc seek=3

# Add the Map and Directory and build the program for adding files
dd if=map.img of=floppya.img bs=512  count=1 seek=1 conv=notrunc
dd if=dir.img of=floppya.img bs=512  count=1 seek=2 conv=notrunc
gcc loadFile.c -o loadFile

# Load additional files into the floppy
./loadFile shell
./loadFile message.txt
./loadFile tstprg
./loadFile tstpr2
./loadFile phello
bochs -f opsys.bxrc
