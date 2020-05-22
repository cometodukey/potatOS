#include <kernel/multiboot.h>
#include <kernel/lib/kprint.h>
#include <kernel/lib/ctype.h>
#include <kernel/lib/string.h>
#include <kernel/lib/mem.h>
#include <kernel/arch/idle.h>

static int parse_id(char id_out[8], char **stream);
static int hexdigit(char c);
static size_t xstr_to_ul(const char *num);
static char consume(char **stream);

size_t *symlist = NULL;
size_t symlist_len = 0;

void
init_symlist(MultibootModule *kernelsyms) {
    size_t i, num;
    char *base = (char *)kernelsyms->mod_start, *name;
    char **syms = &base;
    char c;
    char xdigit[9];

    size_t counter = 0;

    for (i = kernelsyms->mod_end - kernelsyms->mod_start; i; --i) {
        memset(xdigit, 0, LEN(xdigit));
        if (i - 8) {
            /* parse the address */
            if (parse_id(xdigit, syms) == -1) {
                kputs("kernel.symlist is invalid! expected a base 16 number");
                return;
            }
            num = xstr_to_ul(xdigit);
            /* get the start of the symbol name */
            name = base;
            if ((c = consume(syms)) == ' ') {
                for (; i; --i, ++counter) {
                    kprintf("here %u\r\n", counter);
                    c = consume(syms);
                    if (c == '\n') {
                        memcpy(base, &num, sizeof(num));
                        base[sizeof(num)] = ' ';
                        memmove(base+sizeof(num)+1, name, base-name);
                        base[base-name] = '\0';
                        continue;
                    } else if (!isprint(c)) {
                        kputs("kernel.symlist is invalid! expected a printable character or LF");
                        kprintf("%x %c\r\n", c, c);
                        return;
                    }
                }
                kputs("kernel.symlist is invalid! expected a new line");
                return;
            } else {
                kputs("kernel.symlist is invalid! expected a space");
                return;
            }
        }
    }

    symlist = (size_t *)kernelsyms->mod_start;
    symlist_len = kernelsyms->mod_end - kernelsyms->mod_start;
}

static int
parse_id(char id_out[8], char **stream) {
    size_t i;
    char digit;
    for (i = 0; i < 8; ++i) {
        digit = consume(stream);
        if (isxdigit(digit)) {
            id_out[i] = digit;
        } else {
            return -1;
        }
    }
    return 0;
}

static int
hexdigit(char c) {
    if (c >= 'a' && c <= 'f') {
        return (c - 'a') + 10;
    } else if (c >= 'A' && c <= 'F') {
        return (c - 'A') + 10;
    } else if (isdigit(c)) {
        return (c - '0');
    } else {
        return -1;
    }
}

static size_t
xstr_to_ul(const char *str) {
    size_t num = 0;
    for (; *str != '\0'; ++str) {
        num = num * 16 + hexdigit(*str);
    }
    return num;
}

static char
consume(char **stream) {
    char curr = **stream;
    (*stream)++;
    return curr;
}
