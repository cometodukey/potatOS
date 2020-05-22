#include <kernel/types.h>
#include <kernel/varargs.h>
#include <kernel/console/vt100.h>
#include <kernel/lib/kprint.h>
#include <kernel/lib/mem.h>
#include <kernel/lib/assert.h>
#include <kernel/lib/string.h>

// TODO: print type (normal, error, etc), print timestamp

void kprint_flush(void);
void kputchar(char c);
void kputs(const char *s);
void kprint(const char *s);
void kprintf(const char *fmt, ...);
void kvprintf(const char *fmt, va_list args);
static void kprint_to_buf(const char *s);
static void kprint_int(ssize_t num);
static void kprint_uint(size_t num);
static void kprint_hex(size_t num, int prefix);

static char kprint_buf[KPRINT_BUF_SIZE] = {0};

void
kprint_flush(void) {
    kprint_assert(kprint_buf[LEN(kprint_buf)-1] == '\0');
    vt100_puts(kprint_buf);
    memset(kprint_buf, 0, LEN(kprint_buf)-1);
}

void
kputchar(char c) {
    size_t len = strlen(kprint_buf);
    if (len == LEN(kprint_buf)-1) {
        kprint_flush();
    }
    kprint_buf[len] = c;
    if (c == '\n') {
        kprint_flush();
    }
}

void
kputs(const char *s) {
    kprint_to_buf(s);
    kprint_to_buf("\r\n");
}

void
kprint(const char *s) {
    kprint_to_buf(s);
}

void
kprintf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    kvprintf(fmt, args);
    va_end(args);
}

void
kvprintf(const char *fmt, va_list args) {
    const char *va_str;
    char va_char;
    int va_int;
    unsigned int va_uint;
    void *va_ptr;

    for (; *fmt != '\0' ; ++fmt) {
        if (*fmt != '%') {
            kputchar(*fmt);
        } else {
            /* format string */
            switch (*++fmt) {
            case '\0':
                return;
            case '%':
                kputchar('%');
                break;
            case 's':
                va_str = va_arg(args, const char *);
                kprint(va_str == NULL ? "(null)" : va_str);
                break;
            case 'c':
                va_char = (char)va_arg(args, int);
                kputchar(va_char);
                break;
            case 'd':
                va_int = va_arg(args, int);
                kprint_int(va_int);
                break;
            case 'u':
                va_uint = va_arg(args, unsigned int);
                kprint_uint(va_uint);
                break;
            case 'x':
                va_uint = va_arg(args, unsigned int);
                kprint_hex(va_uint, 0);
                break;
            case 'p':
                va_ptr = va_arg(args, void *);
                kprint_hex((size_t)va_ptr, 1);
                break;
            }
        }
    }
}

static void
kprint_to_buf(const char *s) {
    size_t len = strlen(kprint_buf);

    for (; *s != '\0'; ++s, ++len) {
        /* flush buffer is full */
        if (len == LEN(kprint_buf)-1) {
            kprint_flush();
            len = 0;
        }
        kprint_buf[len] = *s;
        /* flush on \n */
        if (*s == '\n') {
            kprint_flush();
            len = 0;
        }
    }
}

static void
kprint_int(ssize_t num) {
    char buf[21] = {0};
    int i, sign;

    if (num == 0) {
        kputchar('0');
        return;
    }

    if ((sign = num < 0)) {
        num = -num;
    }

    for (i = 19; num; --i) {
        buf[i] = (num % 10) + 0x30;
        num = num / 10;
    }
    sign ? buf[i] = '-' : ++i;

    kprint(buf + i);
}

static void
kprint_uint(size_t num) {
    char buf[21] = {0};
    int i;

    if (num == 0) {
        kputchar('0');
        return;
    }

    for (i = 19; num; i--) {
        buf[i] = (num % 10) + 0x30;
        num = num / 10;
    }

    i++;
    kprint(buf + i);
}

static void
kprint_hex(size_t num, int prefix) {
    const char *table = "0123456789abcdef";
    char buf[] = "0000000000000000";
    int i = 15;

    if (num) {
        for (; num; i--) {
            buf[i] = table[(num % 16)];
            num /= 16;
        }
        i++;
    }

    if (prefix) {
        kprint("0x");
    }
    kprint(buf + i);
}
