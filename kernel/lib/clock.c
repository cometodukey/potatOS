#include <kernel/types.h>

size_t boot_timestap;

// TODO: super lazy - use timer interrupts
void
sleep(size_t s) {
    size_t i;
    for (i = 0; i < (s * 10000); ++i) {
        __asm__ volatile ("nop;");
    }
}
