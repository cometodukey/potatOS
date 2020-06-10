#include <kernel/types.h>
#include <kernel/arch/i686/isr/interrupts.h>

void
arch_idle(void) {
    __asm__ volatile (
        "hlt;"
    );
}

noreturn void
arch_hang(void) {
    for (;;) {
        cli();
        arch_idle();
    }
}
