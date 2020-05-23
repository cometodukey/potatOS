#include <kernel/multiboot.h>
#include <kernel/lib/kprint.h>
#include <kernel/lib/ctype.h>
#include <kernel/lib/string.h>
#include <kernel/lib/mem.h>
#include <kernel/arch/idle.h>
#include <kernel/lib/assert.h>

#include <kernel/lib/printf/printf.h>

static int hexdigit(char c);
static void debug_stream(char **stream);
/* static size_t xstr_to_ul(const char *num); */
static char consume(char **stream);
static char expect_consume(char expectation, char **stream);
static int parse_id(char **stream);
static int parse_sym_name(char sym_out_buf[64], char **stream);

size_t *symlist = NULL;
size_t symlist_len = 0;

void
init_symlist(MultibootModule *kernelsyms) {
    UNUSED(debug_stream);
    size_t num;
    char *base = (char *)kernelsyms->mod_start;
    char **syms = &base;
    char sym_name[64];

    for (;hexdigit(*syms[0]) >= 0;) {
        num = parse_id(syms);
        expect_consume(' ', syms);
        parse_sym_name(sym_name, syms);
        expect_consume('\n', syms);
        kprintf("%d => %s\r\n", num, sym_name); // doesn't work due to some weird va_args stuff
    }

    symlist = (size_t *)kernelsyms->mod_start;
    symlist_len = kernelsyms->mod_end - kernelsyms->mod_start;
}

static int
parse_id(char **stream) {
    size_t accum = 0;
    char curr_digit;
    do {
        curr_digit = consume(stream);
        accum = accum * 16 + hexdigit(curr_digit);
    } while (hexdigit((*stream)[0]) >= 0);
    return accum;
}

static int
parse_sym_name(char sym_out_buf[64], char **stream) {
    size_t write_head = 0;
    char curr_char;
    memset(sym_out_buf, '\0', 64);

    do {
        curr_char = consume(stream);
        sym_out_buf[write_head++] = curr_char;
    } while (isprint((*stream)[0]) && (*stream)[0] != '\n');
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

/* static size_t */
/* xstr_to_ul(const char *str) { */
/*     size_t num = 0; */
/*     for (; *str != '\0'; ++str) { */
/*         num = num * 16 + hexdigit(*str); */
/*     } */
/*     return num; */
/* } */

static char
consume(char **stream) {
    char curr = **stream;
    (*stream)++;
    return curr;
}

static char
expect_consume(char expectation, char **stream) {
    const char consumed = consume(stream);
    assert(consumed == expectation);
    return consumed;
}


static void
debug_stream(char **stream) {
    kprintf("WHEAD %m\r\n", *stream, 16);
}
