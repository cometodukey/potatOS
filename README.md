# potatOS
[![Actions Status](https://github.com/cometodukey/potatOS/workflows/nix-build/badge.svg?branch=develop)](https://github.com/cometodukey/potatOS/actions)

potatOS is a small operating system for i686.

Documentation can be found [here](docs/README.md).

## Features

* Multiboot 0.6.96
* An almost standards compliant printf
* Stack trace (with symbols!)
* Serial
* 8259 PIC
* VGA text mode console
* Physical memory allocator

## TODO

* PAE Paging
* APIC
* SMP
* IPC
* Scheduling
* KASLR and ASLR
* initramfs
* Userspace
* Kernel build config
* PIE/PIC
* Tidy up the Makefile
* Handle exceptions better
* Ensure panic/assert cannot receive interrupts
