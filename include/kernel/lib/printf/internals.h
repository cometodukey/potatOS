#ifndef __PRINTF_INTERNALS_H__
#define __PRINTF_INTERNALS_H__

#include <kernel/types.h>

typedef struct s_writer {
    void (*write)(struct s_writer *self, char *str, size_t length);
    void *state;
    size_t written;
    int failed;
} TokenWriter;

void writer_write(TokenWriter *writer, char *str, size_t length);
ssize_t vwprintf(TokenWriter *writer, char *fmt, va_list vlist);

typedef enum {
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
} TokenDescriptor;

typedef short TokenFlags;

#define FLAGS_ZEROPAD      (1 << 0)
#define FLAGS_LEFTALIGN    (1 << 1)
#define FLAGS_PLUS         (1 << 2)
#define FLAGS_SPACE        (1 << 3)
#define FLAGS_HASH         (1 << 4)
#define FLAGS_PRECISION    (1 << 5)
#define FLAGS_CAPITAL      (1 << 6)

#define DEFERRED_WILDCARD (-1)

typedef enum {
    E_HH,
    E_H,
    E_N,
    E_L,
    E_LL,
} TokenSize;

typedef struct {
    TokenDescriptor type;
    TokenFlags flags;
    int width;
    TokenSize size;
    int precision;
    char *s_value;
    size_t s_length;
} Token;

int parse_token(Token *dest, char **stream);
void run_token(TokenWriter *writer, va_list *vlist, Token *token);
/* void debug_token(Token *token); */

/*
** Write to console
*/
typedef void TokenWriterConsoleState;
void writer_console_write(TokenWriter *self, char *str, size_t length);


/*
** Write to serial
*/
typedef void TokenWriterSerialState;
void writer_serial_write(TokenWriter *self, char *str, size_t length);
/*
** Write to string
*/
typedef struct {
    char *str_ptr;
    ssize_t maximum;
    size_t actually_written;
} TokenWriterStringState;
void writer_string_write(TokenWriter *self, char *str, size_t length);


/*---- FORMATTERS ----*/

typedef struct {
    char sign;
    unsigned long value; // FIXME: should really be unsigned long long
    unsigned char base;
} FormatterNumber;

typedef void (*TokenFormatter)(TokenWriter *writer, Token *token, va_list *vlist);

void fmt_putstr(TokenWriter *writer, Token *token, va_list *vlist);
void fmt_putstrlit(TokenWriter *writer, Token *token, va_list *vlist);
void fmt_putpercent(TokenWriter *writer, Token *token, va_list *vlist);
void fmt_putnbr(TokenWriter *writer, Token *token, va_list *vlist);
void fmt_putuns(TokenWriter *writer, Token *token, va_list *vlist);
void fmt_putptr(TokenWriter *writer, Token *token, va_list *vlist);
void fmt_puthex(TokenWriter *writer, Token *token, va_list *vlist);
void fmt_putoct(TokenWriter *writer, Token *token, va_list *vlist);
void fmt_putchr(TokenWriter *writer, Token *token, va_list *vlist);
/* void fmt_putflt(TokenWriter *writer, Token *token, va_list vlist); */
void fmt_printmemory(TokenWriter *writer, Token *token, va_list *vlist);

void intern_fmt_puthex(TokenWriter *writer, unsigned long long value, int caps);
void intern_fmt_pad(TokenWriter *writer, char c, int amt);
char intern_to_hex(char num, int caps);
void intern_pop_wildcards(Token *token, va_list *vlist);
void intern_fmt_pad_auto(TokenWriter *writer, char c, int pad_amount, int length);
void intern_fmt_pad_left(TokenWriter *writer, Token *token, char c, int length);
void intern_fmt_pad_right(TokenWriter *writer, Token *token, char c, int length);
int intern_hex_size(long long n);
size_t intern_ntoa(char *buf_out, FormatterNumber number, int caps);
char intern_pad_char(TokenFlags flags);
unsigned long intern_abs(long source);
unsigned long intern_read_unsigned_int(TokenSize size, va_list *vlist);
long intern_read_signed_int(TokenSize size, va_list *vlist);
/* long double intern_read_float(TokenSize size, va_list vlist); */

char *intern_sign_for(FormatterNumber *number);
void intern_number_prefix(TokenWriter *writer, Token *token, FormatterNumber n, size_t size);

/*---- PARSER ----*/

typedef struct s_char2descriptor {
    char c;
    TokenDescriptor descriptor;
} t_char2descriptor;

typedef struct s_str2size {
    char *str;
    TokenSize size;
} t_str2size;

int parse_atoi(char **stream);
int parse_atoi_wildcard(char **stream);
void truncate_flags(Token *dest);
int parse_precision(TokenFlags *flags, char **stream);
int parse_width(char **stream);
TokenDescriptor parse_conversion_specifier(TokenFlags *flags, char **stream, int *has_errored);
TokenSize parse_size(char **stream);
int is_conversion_specification(const char *stream);


#endif // __PRINTF_INTERNALS_H__
