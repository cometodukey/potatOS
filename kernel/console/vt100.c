#include <kernel/console/console.h>

/* TODO - these functions on exist because of the __kprintf_assert_fail function */

static void vt100_parse_seq(const char **ptr);

static uint8_t old_colour = (BLACK << 4) | WHITE;

void
vt100_putchar(char c) {
    console_write_cell(old_colour, c);
}

void
vt100_puts(const char *s) {
    for (; *s != '\0'; ++s) {
        if (*s == '\033') {
            vt100_parse_seq(&s);
            continue;
        }
        console_write_cell(old_colour, *s);
    }
}

static void
vt100_parse_seq(const char **ptr) {
    const char *seq = *ptr;
    UNUSED(seq); UNUSED(ptr);
}
