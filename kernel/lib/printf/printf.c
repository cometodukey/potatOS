#include <kernel/lib/printf/printf.h>
#include <kernel/lib/printf/internals.h>
#include <kernel/lib/mem.h>
#include <kernel/lib/ctype.h>
#include <kernel/lib/string.h>
#include <kernel/lib/assert.h>
#include <kernel/console/console.h>
#include <kernel/lib/kprint.h>

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) < (y) ? (x) : (y))

ssize_t
vwprintf(t_writer *writer, char *fmt, va_list vlist) {
  int res;
  t_token token;
  
  for (;;)
  {
    res = parse_token(&token, &fmt);
    if (res < 0)
      return res;
    run_token(writer, vlist, &token);
    if (writer->failed)
      return -1;
    if (*fmt == '\0')
      return writer->written;
  }
  return writer->written;
}

ssize_t
vkprintf(char *fmt, va_list vlist) {
  t_writer writer;

  memset(&writer, 0, sizeof(t_writer));
  writer.write = &writer_console_write;
  return vwprintf(&writer, fmt, vlist);
}

ssize_t
kprintf(char *fmt, ...) {
  va_list vlist;
  ssize_t ret;

  va_start(vlist, fmt);
  ret = vkprintf(fmt, vlist);
  va_end(vlist);
  
  return ret;
}


ssize_t vsnprintf(char *dest, ssize_t capacity, char *fmt, va_list vlist)
{
    t_writer_string_state st;
    t_writer writer;
    ssize_t ret;

    st.maximum = capacity > 0 ? capacity - 1 : capacity;
    st.str_ptr = dest;
    st.actually_written = 0;
    memset(&writer, 0, sizeof(t_writer));
    writer.state = (void*)&st;
    writer.write = &writer_string_write;
    ret = vwprintf(&writer, fmt, vlist);
    if (capacity != 0)
        dest[st.actually_written] = '\0';
    return (ret);
}

ssize_t snprintf(char *dest, ssize_t capacity, char *fmt, ...)
{
    va_list vlist;
    ssize_t ret;

    va_start(vlist, fmt);
    ret = vsnprintf(dest, capacity, fmt, vlist);
    va_end(vlist);
    return ret;
}

void
writer_write(t_writer *writer, char *str, size_t length) {
    writer->write(writer, str, length);
}


/*---- WRITING TO CONSOLE ----*/

#define KPRINTF_CONSOLE_BUFFER_SIZE 128

static size_t kprintf_console_buffer_size = 0;
static char kprintf_console_buffer[KPRINTF_CONSOLE_BUFFER_SIZE] = {0};

void writer_console_flush(void) {
  for(size_t i = 0; i < kprintf_console_buffer_size; i++)
    console_write_cell(MAKE_COLOUR(WHITE, BLACK), kprintf_console_buffer[i]);
  memset(kprintf_console_buffer, 0, sizeof(kprintf_console_buffer));
  kprintf_console_buffer_size = 0;
}

void
writer_console_write(t_writer *self, char *str, size_t length) {
  UNUSED(self);
  for(size_t i = 0; i < length; i++) {
    kprintf_console_buffer[kprintf_console_buffer_size++] = str[i];
    if (str[i] == '\n' || kprintf_console_buffer_size >= KPRINTF_CONSOLE_BUFFER_SIZE)
      writer_console_flush();
  }
}

/*---- WRITING TO STRING ----*/

void
writer_string_write(t_writer *self, char *str, size_t length) {
    t_writer_string_state *state; state = (t_writer_string_state*)self->state;
    size_t to_write;

    self->written += length;
    if ((state->actually_written + length > (size_t)state->maximum)
        && state->maximum >= 0)
    {
        to_write = state->maximum - state->actually_written;
        memcpy(state->str_ptr + state->actually_written, str, to_write);
        state->actually_written += to_write;
    }
    else
    {
        memcpy(state->str_ptr + state->actually_written, str, length);
        state->actually_written += length;
    }
}

/*---- FORMATTERS ----*/

size_t
intern_ntoa(char *buf_out, t_number number, int caps) {
    char buf[128];
    char dig;
    size_t len = 0;

    while (len < 128u)
    {
        dig = number.value % number.base;
        buf[len] = intern_to_hex(dig, caps);
        len++;
        number.value /= number.base;
        if (!number.value)
            break ;
    }
    buf[len] = '\0';
    strrev(buf);
    memcpy(buf_out, buf, len);

    return (len);
}

void
intern_fmt_pad(t_writer *writer, char c, int amt) {
    #define PAD_BUFFER_SIZE 64
    char buf[PAD_BUFFER_SIZE];
    int remaining = amt;
    int to_write;

    while (remaining > 0)
      {
        to_write = remaining > PAD_BUFFER_SIZE ? PAD_BUFFER_SIZE : remaining;
        memset(buf, c, to_write);
        writer_write(writer, buf, to_write);
        remaining -= to_write;
      }
}

void
intern_fmt_pad_auto(t_writer *writer, char c, int pad_amount, int length) {
    if (pad_amount > length)
        intern_fmt_pad(writer, c, pad_amount - length);
}

void
intern_fmt_pad_left(t_writer *writer, t_token *token, char c, int length) {
    if ((token->flags & FLAGS_LEFTALIGN) == 0)
        intern_fmt_pad_auto(writer, c, token->width, length);
}

void
intern_fmt_pad_right(t_writer *writer, t_token *token, char c, int length) {
    if (token->flags & FLAGS_LEFTALIGN)
        intern_fmt_pad_auto(writer, c, token->width, length);
}

void
intern_fmt_puthexchar(t_writer *writer, unsigned char c, t_flags flags) {
    const char b = intern_to_hex(c % 16, flags & FLAGS_CAPITAL);
    const char a = intern_to_hex(c / 16, flags & FLAGS_CAPITAL);

    writer_write(writer, (char*)&a, 1);
    writer_write(writer, (char*)&b, 1);
}

void
intern_fmt_putmemchars(t_writer *writer, unsigned char *bytes, size_t offset, size_t len) {
    const size_t    start = offset;

    while (offset < len && offset - start < 16)
    {
        if (isprint(bytes[offset]))
            writer_write(writer, (char*)&bytes[offset], 1);
        else
            writer_write(writer, ".", 1);
        offset++;
    }
}

void
print_row(t_writer *writer, t_flags flags, unsigned char *bytes, size_t length) {
    size_t index = 0;

    while (index < 16)
    {
        if (index > 0 && index % 2 == 0)
            writer_write(writer, " ", 1);
        if (index < length)
            intern_fmt_puthexchar(writer, bytes[index], flags);
        else
            writer_write(writer, "  ", 2);
        index++;
    }
}

void
intern_fmt_printmemory(t_writer *writer, unsigned char *bytes, size_t len, t_flags flags) {
    size_t row = 0;

    if (bytes == NULL)
        writer_write(writer, "(null-pointer)\n", 15);
    while (row < ((len - 1) / 16) + 1)
    {
        if (flags & FLAGS_HASH)
        {
            intern_fmt_pad_auto(writer, '0', 8, intern_hex_size(row * 16));
            intern_fmt_puthex(writer, row * 16, flags & FLAGS_CAPITAL);
            writer_write(writer, ": ", 2);
        }
        print_row(writer, flags, bytes + (row * 16), len - (row * 16));
        writer_write(writer, " ", 1);
        intern_fmt_putmemchars(writer, bytes, row * 16, len);
        writer_write(writer, "\r\n", 2);
        row++;
    }
}

void
fmt_printmemory(t_writer *writer, t_token *token, va_list vlist) {
    void *addr = va_arg(vlist, void*);
    size_t len = va_arg(vlist, size_t);

    intern_fmt_printmemory(writer, (unsigned char *)addr, len, token->flags);
}


int
intern_hex_size(long long n) {
    int count = 1;

    if (n < 0)
    {
        count++;
        n = -n;
    }
    while (n / 16)
    {
        count++;
        n /= 16;
    }
    return count;
}

void
intern_fmt_puthex(t_writer *writer, unsigned long long value, int caps) {
    char c;

    if (value >= 16)
        intern_fmt_puthex(writer, value / 16, caps);
    c = intern_to_hex(value % 16, caps);
    writer_write(writer, &c, 1);
}

int
prefix_size(t_number *num, t_flags flags) {
    if (flags & FLAGS_HASH && num->value != 0)
        return (2);
    else
        return (0);
}

void
fmt_do_puthex(t_writer *writer, t_token *token, t_number number) {
    unsigned long long own_hex_size;
    char buf[128];
    size_t idx = intern_ntoa(buf, number, token->flags & FLAGS_CAPITAL);

    own_hex_size = prefix_size(&number, token->flags);
    own_hex_size = token->flags & FLAGS_PRECISION
        ? (size_t)MAX(token->precision + own_hex_size, own_hex_size + idx)
        : own_hex_size + idx;
    if (!(token->flags & FLAGS_ZEROPAD))
        intern_fmt_pad_left(writer, token, ' ', own_hex_size);
    if ((token->flags & FLAGS_HASH) && number.value != 0)
        writer_write(writer,
            token->flags & FLAGS_CAPITAL ? "0X" : "0x", 2);
    if (token->flags & FLAGS_ZEROPAD)
        intern_fmt_pad(writer, '0', token->width - (int)own_hex_size);
    if (token->flags & FLAGS_PRECISION && idx < (size_t)token->precision)
        intern_fmt_pad(writer, '0', token->precision - (int)idx);
    writer_write(writer, buf, idx);
    if (!(token->flags & FLAGS_ZEROPAD))
        intern_fmt_pad_right(writer, token, ' ', (int)own_hex_size);
}

void
fmt_puthex(t_writer *writer, t_token *token, va_list vlist) {
    unsigned long long n;
    t_number number;

    intern_pop_wildcards(token, vlist);
    n = intern_read_unsigned_int(token->size, vlist);
    number.value = n;
    number.base = 16U;
    if (number.value == 0 && token->flags & FLAGS_PRECISION && token->precision == 0)
        intern_fmt_pad(writer, ' ', token->width);
    else
        fmt_do_puthex(writer, token, number);
}

unsigned long long
intern_abs(long long source) {
    if (source >= 0)
        return ((unsigned long long)source);
    else
        return (-((unsigned long long)source));
}

void
intern_read_number(t_number *number, t_size size, unsigned char base, va_list vlist) {
    long long value = intern_read_signed_int(size, vlist);

    number->sign = value >= 0 ? 1 : -1;
    number->value = intern_abs(value);
    number->base = base;
}

size_t
calculate_actual_size(const t_token *tok, size_t number_width, const t_number *number) {
    size_t      actual_size;

    if (tok->flags & FLAGS_PRECISION)
        actual_size = MAX((int)number_width, tok->precision);
    else
        actual_size = number_width;
    if (number->sign == -1 || tok->flags & (FLAGS_PLUS | FLAGS_SPACE))
        actual_size++;
    return (actual_size);
}

void
fmt_putnbr(t_writer *writer, t_token *tok, va_list vlist) {
    char buf[128];
    t_number n;
    size_t idx;
    size_t size;

    intern_pop_wildcards(tok, vlist);
    intern_read_number(&n, tok->size, 10U, vlist);
    if (n.value == 0 && tok->flags & FLAGS_PRECISION && tok->precision == 0) {
        intern_fmt_pad(writer, ' ', tok->width);
        return ;
    }
    idx = intern_ntoa(buf, n, 0);
    size = calculate_actual_size(tok, idx, &n);
    intern_number_prefix(writer, tok, n, size);
    if (tok->flags & FLAGS_PRECISION)
        intern_fmt_pad_auto(writer, '0', tok->precision, idx);
    writer_write(writer, buf, idx);
    intern_fmt_pad_right(writer, tok, intern_pad_char(tok->flags), size);
}

void
do_fmt_putoct(t_writer *writer, t_token *token, t_number number)
{
    char buf[128];
    size_t idx = intern_ntoa(buf, number, 0);
    size_t actual_size;

    if (token->flags & FLAGS_HASH && buf[0] != '0' &&
        (!(token->flags & FLAGS_PRECISION) || (size_t)token->precision <= idx))
    {
        token->flags |= FLAGS_PRECISION;
        token->precision = idx + 1;
    }
    actual_size = idx;
    if (token->flags & FLAGS_HASH)
        actual_size++;
    if (token->flags & FLAGS_PRECISION)
        actual_size = MAX((int)actual_size, token->precision);
    intern_fmt_pad_left(writer, token,
        intern_pad_char(token->flags), actual_size);
    if ((token->flags & FLAGS_PRECISION))
        intern_fmt_pad_auto(writer, '0', token->precision, idx);
    writer_write(writer, buf, idx);
    intern_fmt_pad_right(writer, token,
        intern_pad_char(token->flags), actual_size);
}

void
fmt_putoct(t_writer *writer, t_token *token, va_list vlist) {
    t_number num;

    intern_pop_wildcards(token, vlist);
    num.value = intern_read_unsigned_int(token->size, vlist);
    num.base = 8U;
    if (num.value == 0
        && token->flags & FLAGS_PRECISION
        && token->precision == 0)
    {
        intern_fmt_pad(writer, ' ', token->width -
            !!(token->flags & FLAGS_HASH));
        if (token->flags & FLAGS_HASH)
            writer_write(writer, "0", 1);
    }
    else
        do_fmt_putoct(writer, token, num);
}

void
fmt_putpercent(t_writer *writer, t_token *token, va_list vlist) {
    UNUSED(vlist);
    if ((token->flags & FLAGS_LEFTALIGN) == 0)
        intern_fmt_pad(writer, ' ', MAX(0, token->width - 1));
    writer_write(writer, "%", 1);
    if (token->flags & FLAGS_LEFTALIGN)
        intern_fmt_pad(writer, ' ', MAX(0, token->width - 1));
}

void
fmt_putptr(t_writer *writer, t_token *token, va_list vlist) {
    unsigned long long  actual_size;
    unsigned long long  n;
    t_number            number;
    char                buf[128];
    size_t              idx;

    intern_pop_wildcards(token, vlist);
    n = (unsigned long long)va_arg(vlist, void *);
    number.value = n;
    number.base = 16U;
    idx = intern_ntoa(buf, number, token->flags & FLAGS_CAPITAL);
    actual_size = 2;
    actual_size += idx;
    actual_size = token->flags & FLAGS_PRECISION
        ? (size_t)MAX(token->precision, (int)actual_size)
        : actual_size;
    intern_fmt_pad_left(writer, token, ' ', actual_size);
    writer_write(writer, token->flags & FLAGS_CAPITAL ? "0X" : "0x", 2);
    if (token->flags & FLAGS_PRECISION && idx < (size_t)token->precision)
        intern_fmt_pad(writer, '0', token->precision - idx);
    writer_write(writer, buf, idx);
    intern_fmt_pad_right(writer, token, ' ', actual_size);
}

void
fmt_putstr(t_writer *writer, t_token *token, va_list vlist) {
    char    *str;
    size_t  len;
    size_t  trimmed_length;
    char    check;

    intern_pop_wildcards(token, vlist);
    str = va_arg(vlist, char *);
    if (str == NULL)
    {
        check = (!(token->flags & FLAGS_PRECISION) || token->precision >= 6);
        intern_fmt_pad_left(writer, token, ' ', check * 6);
        writer_write(writer, "(null)", check * 6);
        intern_fmt_pad_right(writer, token, ' ', check * 6);
    }
    else
    {
        len = strlen(str);
        if (token->precision < 0)
            token->flags &= ~FLAGS_PRECISION;
        trimmed_length = token->flags & FLAGS_PRECISION
            ? (size_t)MIN((int)len, token->precision) : len;
        intern_fmt_pad_left(writer, token, ' ', trimmed_length);
        writer_write(writer, str, trimmed_length);
        intern_fmt_pad_right(writer, token, ' ', trimmed_length);
    }
}

void
fmt_putstrlit(t_writer *writer, t_token *token, va_list vlist) {
    (void)vlist;
    writer_write(writer, token->s_value, token->s_length);
}

void
fmt_putchr(t_writer *writer, t_token *token, va_list vlist) {
    char        c;

    intern_pop_wildcards(token, vlist);
    c = (char)va_arg(vlist, int);
    if ((token->flags & FLAGS_LEFTALIGN) == 0)
        intern_fmt_pad(writer, ' ', MAX(0, token->width - 1));
    writer_write(writer, &c, 1);
    if (token->flags & FLAGS_LEFTALIGN)
        intern_fmt_pad(writer, ' ', MAX(0, token->width - 1));
}

t_number
va_arg_unsigned_number(t_size size, va_list vlist) {
    t_number            number;
    unsigned long long  input;

    input = intern_read_unsigned_int(size, vlist);
    number.sign = 1;
    number.value = input;
    number.base = 10U;
    return (number);
}

void
fmt_putuns(t_writer *writer, t_token *tok, va_list vlist) {
    char        buf[128];
    t_number    n;
    size_t      idx;
    size_t      actual_size;

    intern_pop_wildcards(tok, vlist);
    n = va_arg_unsigned_number(tok->size, vlist);
    if (n.value == 0
        && tok->flags & FLAGS_PRECISION
        && tok->precision == 0)
    {
        intern_fmt_pad(writer, ' ', tok->width);
        return ;
    }
    idx = intern_ntoa(buf, n, 0);
    actual_size = idx;
    if (tok->flags & FLAGS_PRECISION)
        actual_size = MAX((int)idx, tok->precision);
    intern_fmt_pad_left(writer, tok, intern_pad_char(tok->flags), actual_size);
    if (tok->flags & FLAGS_PRECISION)
        intern_fmt_pad_auto(writer, '0', tok->precision, idx);
    writer_write(writer, buf, idx);
    intern_fmt_pad_right(writer, tok, intern_pad_char(tok->flags), actual_size);
}


/*
** It's impossble to read a size smaller then int with va_arg, since they are
** automaticly promoted to an int. That's why we read them as an int and then
** do a cast dance.
*/

unsigned long long
intern_read_unsigned_int(t_size size, va_list v) {
    if (size == E_HH)
        return ((unsigned long long)(unsigned char)va_arg(v, unsigned int));
    else if (size == E_H)
        return ((unsigned long long)(unsigned short)va_arg(v, unsigned int));
    else if (size == E_N)
        return ((unsigned long long)va_arg(v, unsigned int));
    else if (size == E_L)
        return ((unsigned long long)va_arg(v, unsigned long));
    else
        return (va_arg(v, unsigned long long));
}

long long
intern_read_signed_int(t_size size, va_list vlist) {
  switch (size)
    {
    case E_HH: return ((long long)(char)va_arg(vlist, int));
    case E_H:  return ((long long)(short)va_arg(vlist, int));
    case E_N:  return ((long long)va_arg(vlist, int));
    case E_L:  return ((long long)va_arg(vlist, long));
    default:   return (va_arg(vlist, long long));
    }
}

/* long double */
/* intern_read_float(t_size size, va_list vlist) { */
/*     if (size == E_LL) */
/*         return (va_arg(vlist, long double)); */
/*     else */
/*         return ((long double)va_arg(vlist, double)); */
/* } */

void
intern_pop_wildcards(t_token *token, va_list vlist) {
    if (token->width == DEFERRED_WILDCARD)
    {
        token->width = va_arg(vlist, int);
        if (token->width < 0)
        {
            token->width = -token->width;
            token->flags |= FLAGS_LEFTALIGN;
        }
    }
    if (token->flags & FLAGS_PRECISION && token->precision == DEFERRED_WILDCARD)
        token->precision = va_arg(vlist, int);
}

char
intern_pad_char(t_flags flags) {
    return (flags & FLAGS_ZEROPAD) ? '0' : ' ';
}

char *
intern_sign_for(t_number *number) {
    if (number->sign == -1)
        return ("-");
    if (number->sign == 1)
        return ("+");
    return (NULL);
}

void
intern_number_prefix(t_writer *writer, t_token *token, t_number n, size_t size) {
    if (!(token->flags & FLAGS_ZEROPAD))
        intern_fmt_pad_left(writer, token, intern_pad_char(token->flags), size);
    if (n.sign == -1 || token->flags & FLAGS_PLUS)
        writer_write(writer, intern_sign_for(&n), 1);
    if (n.sign == 1 && (token->flags & FLAGS_SPACE))
        writer_write(writer, " ", 1);
    if (token->flags & FLAGS_ZEROPAD)
        intern_fmt_pad_left(writer, token, intern_pad_char(token->flags), size);
}

char
intern_to_hex(char num, int caps) {
    char hex_starter = caps ? 'A' : 'a';

    if (num >= 10 && num <= 15)
        return hex_starter + num - 10;
    else if (num >= 0 && num <= 9)
        return '0' + num;
    return '\0';
}

void
run_token(t_writer *writer, va_list vlist, t_token *token) {
    const t_formatter fmts[] = {
        fmt_putstrlit,
        fmt_putpercent,
        fmt_putnbr,
        fmt_putstr,
        fmt_putuns,
        fmt_putptr,
        fmt_puthex,
        fmt_putchr,
        fmt_putoct,
        NULL, /* fmt_putflt, */
        fmt_printmemory,
    };

    fmts[token->type](writer, token, vlist);
}


/*---- PARSER ----*/

int
parse_atoi(char **stream) {
    int sum = 0;
    while (isdigit(**stream))
    {
        sum = sum * 10 + (**stream - '0');
        (*stream)++;
    }
    return sum;
}

int
parse_atoi_wildcard(char **stream) {
    if (**stream == '*')
    {
        (*stream)++;
        return DEFERRED_WILDCARD;
    }
    else if (isdigit(**stream))
        return parse_atoi(stream);
    else
        return 0;
}

/*
** Fix token which can be parsed, but have to be corrected.
**
** Relevent C Standard snippets:
**  - For d, i, o, u, x, and X conversions, if a precision is specified, the 0
**    flag is ignored.
**  - If the space and + flags both appear, the space flag is ignored.
**  - If the 0 and - flags both appear, the 0 flag is ignored.
** 
*/

void
truncate_flags(t_token *dest)
{
    if ((dest->type == E_INT || dest->type == E_OCT || dest->type == E_UNS ||
            dest->type == E_HEX) && dest->flags & FLAGS_PRECISION)
        dest->flags &= ~FLAGS_ZEROPAD;
    if (dest->flags & FLAGS_PLUS)
        dest->flags &= ~FLAGS_SPACE;
    if (dest->flags & FLAGS_LEFTALIGN)
        dest->flags &= ~FLAGS_ZEROPAD;
}

int          parse_flags(char **stream, int *has_errored)
{
    t_flags         flags;

    assert(stream != NULL && *stream != NULL);
    flags = 0;
    while (strchr("# 0+-", **stream))
    {
        if (**stream == '#')
            flags |= FLAGS_HASH;
        else if (**stream == ' ')
            flags |= FLAGS_SPACE;
        else if (**stream == '0')
            flags |= FLAGS_ZEROPAD;
        else if (**stream == '+')
            flags |= FLAGS_PLUS;
        else if (**stream == '-')
            flags |= FLAGS_LEFTALIGN;
        else if (**stream == '\0')
            *has_errored = 1;
        else
            assert(0);
        (*stream)++;
    }
    return (flags);
}

const t_str2size g_map_str2size[] = {
    { "hh", E_HH },
    { "h",  E_H  },
    { "ll", E_LL },
    { "l",  E_L  },
    { "L",  E_LL },
    { "",   E_N  },
};

t_size
parse_size(char **stream) {
    size_t i = 0;
    size_t str_len;
    const t_str2size *elem;

    assert(stream != NULL && *stream != NULL);
    while (i < sizeof(g_map_str2size) / sizeof(t_str2size))
    {
        elem = &g_map_str2size[i];
        str_len = strlen(elem->str);
        if (strncmp(*stream, elem->str, str_len) == 0)
        {
            (*stream) += str_len;
            return (elem->size);
        }
        i++;
    }
    assert(0);
}

const t_char2descriptor  g_map_char2descriptor[] = {
    { 'd', E_INT },
    { 'i', E_INT },
    { 's', E_STR },
    { 'o', E_OCT },
    { 'u', E_UNS },
    { 'p', E_PTR },
    { 'c', E_CHR },
    { '%', E_PERCENT },
    { 'm', E_MEMORY },
    { 'x', E_HEX },
    { 'f', E_FLOAT },
};

t_descriptor
parse_conversion_specifier(t_flags *flags, char **stream, int *has_errored) {
    char c;
    size_t i;

    assert(flags != NULL && stream != NULL && *stream != NULL);
    c = **stream;
    (*stream)++;
    if (c == '\0')
    {
        *has_errored = 1;
        return 0;
    }
    if (c >= 'A' && c <= 'Z')
    {
        *flags |= FLAGS_CAPITAL;
        c = tolower(c);
    }
    i = 0;
    while (i < sizeof(g_map_char2descriptor) / sizeof(t_char2descriptor))
    {
        if (g_map_char2descriptor[i].c == c)
            return (g_map_char2descriptor[i].descriptor);
        i++;
    }
    *has_errored = 1;
    return 0;
}

int
parse_width(char **stream) {
    assert(stream != NULL && *stream != NULL);
    return parse_atoi_wildcard(stream);
}

int
parse_precision(t_flags *flags, char **stream) {
    assert(flags != NULL && stream != NULL && *stream != NULL);
    if (**stream != '.')
        return 0;
    (*stream)++;
    *flags |= FLAGS_PRECISION;
    return parse_atoi_wildcard(stream);
}

int
is_conversion_specification(const char *stream) {
    assert(stream != NULL);
    return *stream == '%';
}

int
parse_conversion_specification(t_token *dest, char **stream) {
    int has_errored = 0;

    assert(dest != NULL && stream != NULL && *stream != NULL);
    assert(is_conversion_specification(*stream));
    (*stream)++;
    dest->flags = parse_flags(stream, &has_errored);
    if (has_errored)
        return -1;
    dest->width = parse_width(stream);
    dest->precision = parse_precision(&dest->flags, stream);
    dest->size = parse_size(stream);
    dest->type = parse_conversion_specifier(&dest->flags, stream, &has_errored);
    if (has_errored)
        return -1;
    truncate_flags(dest);
    return **stream != '\0';
}

int
parse_string_literal(t_token *dest, char **stream) {
    assert(dest != NULL && stream != NULL && *stream != NULL);
    assert(!is_conversion_specification(*stream));
    dest->type = E_TXT;
    dest->s_value = *stream;
    while (!is_conversion_specification(*stream) && **stream != '\0')
        (*stream)++;
    dest->s_length = *stream - dest->s_value;
    return **stream != '\0';
}

int
parse_token(t_token *dest, char **stream) {
    assert(dest != NULL && stream != NULL && *stream != NULL);
    memset(dest, '\0', sizeof(t_token));
    if (is_conversion_specification(*stream))
        return parse_conversion_specification(dest, stream);
    else
        return parse_string_literal(dest, stream);
}
