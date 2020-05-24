#include <kernel/multiboot.h>
#include <kernel/lib/kprint.h>
#include <kernel/lib/ctype.h>
#include <kernel/lib/string.h>
#include <kernel/lib/mem.h>
#include <kernel/arch/idle.h>
#include <kernel/lib/assert.h>
#include <kernel/lib/symbols.h>
#include <kernel/lib/errno.h>

static ssize_t hexdigit(char c);
static char consume(char **stream);
static char expect_consume(char expectation, char **stream);
static size_t parse_id(char **stream);
static KernelResult parse_sym_name(char sym_out_buf[SYM_NAME_SIZ], char **stream);

size_t
parse_symlist(MultibootModule *kernelsyms, char sym_name[SYM_NAME_SIZ], size_t addr) {
    size_t num, prev = 0;
    char *base = (char *)kernelsyms->mod_start;
    char **syms = &base;
    char sym_name_buf[SYM_NAME_SIZ] = {'\0'};

    for (; hexdigit(*syms[0]) >= 0;) {
        num = parse_id(syms);
        if (num > addr) {
            memcpy(sym_name, sym_name_buf, SYM_NAME_SIZ);
            return prev;
        }
        expect_consume(' ', syms);
        if (parse_sym_name(sym_name_buf, syms) == GENERIC_ERR) {
            memcpy(sym_name_buf, "?", 2);
            break;
        }
        expect_consume('\n', syms);
        prev = num;
    }
    memcpy(sym_name, sym_name_buf, SYM_NAME_SIZ);
    return num;
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

static KernelResult
parse_sym_name(char sym_out_buf[SYM_NAME_SIZ], char **stream) {
    size_t write_head = 0;
    char curr_char;
    memset(sym_out_buf, '\0', 64);

    for (; write_head < SYM_NAME_SIZ;) {
        if ((*stream)[0] != '\n') {
            if (!isprint((*stream)[0])) {
                return GENERIC_ERR;
            }
            curr_char = consume(stream);
            sym_out_buf[write_head++] = curr_char;
        } else {
            break;
        }
    }

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
        return GENERIC_ERR;
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
