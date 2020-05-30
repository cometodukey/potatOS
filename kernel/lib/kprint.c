#include <kernel/types.h>
#include <kernel/varargs.h>
#include <kernel/console/vt100.h>
#include <kernel/lib/kprint.h>
#include <kernel/lib/mem.h>
#include <kernel/lib/assert.h>
#include <kernel/lib/string.h>

// void kprint_flush(void);
// void kputchar(char c);
void kputs(const char *s);

// static char kprint_buf[KPRINT_BUF_SIZE] = {0};
// void
// kprint_flush(void) {
//     kprint_assert(kprint_buf[LEN(kprint_buf)-1] == '\0');
//     vt100_puts(kprint_buf);
//     memset(kprint_buf, 0, LEN(kprint_buf)-1);
// }

// void
// kputchar(char c) {
//     size_t len = strlen(kprint_buf);
//     if (len == LEN(kprint_buf)-1) {
//         kprint_flush();
//     }
//     kprint_buf[len] = c;
//     if (c == '\n') {
//         kprint_flush();
//     }
// }

void
kputs(const char *s) {
    kprintf("%s\r\n", s);
}

void
kprint(const char *s) {
    kprintf("%s", s);
}
