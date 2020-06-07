#include <kernel/types.h>

void
idle(void) {
    __asm__ volatile (
        "cli; hlt;"
    );
}

noreturn void
hang(void) {
    for (;;) {
        idle();
    }
}
