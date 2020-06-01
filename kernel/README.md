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

* ACPI, AML, MADT
* Stack protections (NX, stack overflow, buffer overflow)?
* Rewrite the console code
* VMM and paging
* Facilitate IPC
* Scheduling
* KASLR and ASLR
* initramfs
* Userspace
