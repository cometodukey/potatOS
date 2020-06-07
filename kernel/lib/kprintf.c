#include <kernel/lib/kprintf.h>

void
kprint_flush(void) {
    /* FIXME - super lazy, need to write a better function */
    kprint("\n");
}

void
kputs(const char *s) {
    kprintf("%s\r\n", s);
}

void
kprint(const char *s) {
    kprintf("%s", s);
}
