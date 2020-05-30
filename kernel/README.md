# Spud

Spud is an i686 kernel for the potatOS operating system.

## Features

* Multiboot 0.6.96
* An almost standards compliant printf
* Stack trace (with symbols!)
* Serial
* 8259 PIC
* APIC
* VGA text mode console
* Physical memory allocator

## TODO

* Stack protections (NX, stack overflow, buffer overflow)?
* Rewrite the console code
* VMM and paging
* Facilitate IPC
* Scheduling
* KASLR and ASLR
* initramfs
* Userspace
* Kernel module signing? comrpession?
* Direct device io for drivers?
* Be able to restart system servers

## Modules

* kernel.symlist - not required but useful for stack traces
* initramfs - required to boot the system
