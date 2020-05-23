#ifndef __PRINTF_INTERNALS_H__
#define __PRINTF_INTERNALS_H__

#include <kernel/types.h>
#include <kernel/varargs.h>

typedef struct s_writer {
    void (*write)(struct s_writer *self, char *str, size_t length);
    void *state;
    size_t written;
    int failed;
} t_writer;

void writer_write(t_writer *writer, char *str, size_t length);
ssize_t vwprintf(t_writer *writer, char *fmt, va_list vlist);

typedef enum e_descriptor {
    E_TXT,
    E_PERCENT,
    E_INT,
    E_STR,
    E_UNS,
    E_PTR,
    E_HEX,
    E_CHR,
    E_OCT,
    E_FLOAT,
    E_MEMORY,
} t_descriptor;

typedef short t_flags;

#define FLAGS_ZEROPAD      (1 << 0)
#define FLAGS_LEFTALIGN    (1 << 1)
#define FLAGS_PLUS         (1 << 2)
#define FLAGS_SPACE        (1 << 3)
#define FLAGS_HASH         (1 << 4)
#define FLAGS_PRECISION    (1 << 5)
#define FLAGS_CAPITAL      (1 << 6)

#define DEFERRED_WILDCARD (-1)

typedef enum e_size {
    E_HH,
    E_H,
    E_N,
    E_L,
    E_LL,
} t_size;

typedef struct s_token {
    t_descriptor type;
    t_flags flags;
    int width;
    t_size size;
    int precision;
    char *s_value;
    size_t s_length;
} t_token;

int parse_token(t_token *dest, char **stream);
void run_token(t_writer *writer, va_list *vlist, t_token *token);
/* void debug_token(t_token *token); */

/*
** Write to console
*/
typedef void t_writer_console_state;
void writer_console_write(t_writer *self, char *str, size_t length);


/*
** Write to serial
*/
typedef void t_writer_serial_state;
void writer_serial_write(t_writer *self, char *str, size_t length);
/*
** Write to string
*/
typedef struct s_writer_string_state {
    char *str_ptr;
    ssize_t maximum;
    size_t actually_written;
} t_writer_string_state;
void writer_string_write(t_writer *self, char *str, size_t length);


/*---- FORMATTERS ----*/

typedef struct s_number {
    char sign;
    unsigned long value; // FIXME: should really be unsigned long long
    unsigned char base;
} t_number;

typedef void (*t_formatter)(t_writer *writer, t_token *token, va_list *vlist);

void fmt_putstr(t_writer *writer, t_token *token, va_list *vlist);
void fmt_putstrlit(t_writer *writer, t_token *token, va_list *vlist);
void fmt_putpercent(t_writer *writer, t_token *token, va_list *vlist);
void fmt_putnbr(t_writer *writer, t_token *token, va_list *vlist);
void fmt_putuns(t_writer *writer, t_token *token, va_list *vlist);
void fmt_putptr(t_writer *writer, t_token *token, va_list *vlist);
void fmt_puthex(t_writer *writer, t_token *token, va_list *vlist);
void fmt_putoct(t_writer *writer, t_token *token, va_list *vlist);
void fmt_putchr(t_writer *writer, t_token *token, va_list *vlist);
/* void fmt_putflt(t_writer *writer, t_token *token, va_list vlist); */
void fmt_printmemory(t_writer *writer, t_token *token, va_list *vlist);

void intern_fmt_puthex(t_writer *writer, unsigned long long value, int caps);
void intern_fmt_pad(t_writer *writer, char c, int amt);
char intern_to_hex(char num, int caps);
void intern_pop_wildcards(t_token *token, va_list *vlist);
void intern_fmt_pad_auto(t_writer *writer, char c, int pad_amount, int length);
void intern_fmt_pad_left(t_writer *writer, t_token *token, char c, int length);
void intern_fmt_pad_right(t_writer *writer, t_token *token, char c, int length);
int intern_hex_size(long long n);
size_t intern_ntoa(char *buf_out, t_number number, int caps);
char intern_pad_char(t_flags flags);
unsigned long intern_abs(long source);
unsigned long intern_read_unsigned_int(t_size size, va_list *vlist);
long intern_read_signed_int(t_size size, va_list *vlist);
/* long double intern_read_float(t_size size, va_list vlist); */

char *intern_sign_for(t_number *number);
void intern_number_prefix(t_writer *writer, t_token *token, t_number n, size_t size);

/*---- PARSER ----*/

typedef struct s_char2descriptor {
    char c;
    t_descriptor descriptor;
} t_char2descriptor;

typedef struct s_str2size {
    char *str;
    t_size size;
} t_str2size;

int parse_atoi(char **stream);
int parse_atoi_wildcard(char **stream);
void truncate_flags(t_token *dest);
int parse_precision(t_flags *flags, char **stream);
int parse_width(char **stream);
t_descriptor parse_conversion_specifier(t_flags *flags, char **stream, int *has_errored);
t_size parse_size(char **stream);
int is_conversion_specification(const char *stream);


#endif // __PRINTF_INTERNALS_H__
