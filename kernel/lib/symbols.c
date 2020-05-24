#include <kernel/multiboot.h>
#include <kernel/lib/kprint.h>
#include <kernel/lib/ctype.h>
#include <kernel/lib/string.h>
#include <kernel/lib/mem.h>
#include <kernel/arch/idle.h>
#include <kernel/lib/assert.h>
#include <kernel/lib/symbols.h>

static ssize_t hexdigit(char c);
static char consume(char **stream);
static char expect_consume(char expectation, char **stream);
static size_t parse_id(char **stream);
static int parse_sym_name(char sym_out_buf[64], char **stream);

size_t
parse_symlist(MultibootModule *kernelsyms, char sym_name[SYM_NAME_SIZ], size_t addr) {
    size_t num, prev = 0;
    char *base = (char *)kernelsyms->mod_start;
    char **syms = &base;

    for (; hexdigit(*syms[0]) >= 0;) {
        num = parse_id(syms);
        expect_consume(' ', syms);
        parse_sym_name(sym_name, syms);
        expect_consume('\n', syms);
        kprintf("addr %x | num %x | prev %x\r\n", addr, num, prev);
        if (num > addr) {
            return prev;
        }
        prev = num;
    }
    memcpy(sym_name, "?", 2);
    return addr;
}

static size_t
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
parse_sym_name(char sym_out_buf[SYM_NAME_SIZ], char **stream) {
    size_t write_head = 0;
    char curr_char;
    memset(sym_out_buf, '\0', 64);

    do {
        curr_char = consume(stream);
        sym_out_buf[write_head++] = curr_char;
    } while (write_head < SYM_NAME_SIZ &&
                isprint((*stream)[0]) && (*stream)[0] != '\n');
    return 0;
}

static ssize_t
hexdigit(char c) {
    if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    } else if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    } else if (isdigit(c)) {
        return c - '0';
    } else {
        return -1;
    }
}

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
