#include <kernel/types.h>

void
arch_idle(void) {
    __asm__ volatile (
        "hlt;"
    );
}

noreturn void
arch_hang(void) {
    for (;;) {
        arch_idle();
    }
}
