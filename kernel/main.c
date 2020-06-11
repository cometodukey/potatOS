#include <kernel/arch/arch.h>
#include <kernel/types.h>
#include <kernel/arch/multiboot.h>
#include <kernel/lib/kprintf.h>
#include <kernel/lib/panic.h>
#include <kernel/lib/task.h>
#include <kernel/lib/kmalloc.h>

const char *version = xstringify(KERNEL_VERSION);

noreturn void
kernel_main(uint32_t magic, const MultibootInfo *mb) {
    extern MultibootModule *initramfs;
    extern const char *cmdline;

    /* initialise architecture specific features first thing */
    init_arch(magic, mb);

    kprintf("spud-%s started\r\n", version);
    kputs("MIT License. Copyright (c) 2020 Edward Bruce");

    if (cmdline != NULL) {
        kprintf("Command line: %s\r\n", cmdline);
    }
    if (initramfs == NULL) {
        PANIC("No initramfs was loaded!");
    }

    /* initialse the scheduler and kmalloc */
    init_kmalloc();
    init_task();

    /* spawn the bootstrap server */
    // TODO

    /* idle loop - wait for interrupts */
    for (;;) {
        idle();
    }
}
