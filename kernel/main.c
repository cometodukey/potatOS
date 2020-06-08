#include <kernel/types.h>
#include <kernel/multiboot.h>
#include <kernel/lib/clock.h>
//#include <kernel/lib/assert.h>
#include <kernel/console/console.h>
#include <kernel/lib/kprintf.h>
#include <kernel/i686/idle.h>
#include <kernel/i686/gdt.h>
#include <kernel/i686/interrupts.h>
#include <kernel/lib/panic.h>
#include <kernel/lib/random.h>
#include <kernel/mm/pmm.h>
//#include <kernel/lib/symbols.h>
#include <kernel/serial/serial.h>

const char *version = xstringify(KERNEL_VERSION);
MultibootModule *initramfs = NULL;
MultibootModule *kernel_syms = NULL;

noreturn void
kernel_main(uint32_t magic, const MultibootInfo *mb) {
    //TODO: boot_timestamp = get_timestamp();
    init_random();
    init_console();
    init_serial(SERIAL_COM1_BASE);

    kprintf("spud-%s started\r\n", version);
    kputs("MIT License. Copyright (c) 2020 Edward Bruce\r\n");
    // TODO: log the boot time

    kputs("Loading descriptor tables");
    init_gdt();
    init_idt();

    parse_multiboot_info(magic, mb);
    if (initramfs == NULL) {
        PANIC("No initramfs was loaded!");
    }
    if (kernel_syms == NULL) {
        kputs("No kernel symbol table was loaded");
    }

    init_pmm(mb->mmap_addr, mb->mmap_length);

    hang();
}
