target remote localhost:1234
symbol-file fsroot/boot/spud-0.1.0.elf
b main.c:kernel_main
set architecture i386:x86-64:intel
