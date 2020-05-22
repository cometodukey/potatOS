#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <kernel/types.h>

#define CONSOLE_TAB_WIDTH 8
#define MAKE_COLOUR(fg, bg) ((bg << 4) | fg)
#define CONSOLE_CURSOR_POS \
    ((console.cursor.row * console.cols) + console.cursor.col)
#define CONSOLE_BYTES_PER_CHAR ((console.bpp) / (__CHAR_BIT__))
#define CONSOLE_ROW(n) ((n) * (console.cols))
#define CONSOLE_BUF_SIZE ((console.rows) * (console.cols) * (CONSOLE_BYTES_PER_CHAR))

enum VGAConsoleColours {
    BLACK = 0,
    BLUE,
    GREEN,
    CYAN,
    RED,
    MAGENTA,
    BROWN,
    LIGHT_GREY,
    DARK_GREY,
    LIGHT_BLUE,
    LIGHT_GREEN,
    LIGHT_CYAN,
    LIGHT_RED,
    LIGHT_MAGENTA,
    YELLOW,
    WHITE
};

typedef struct {
    size_t col;
    size_t row;
} ConsoleCursor;

typedef struct {
    uint16_t *addr;
    size_t bpp;
    size_t pitch;
    size_t cols;
    size_t rows;
    ConsoleCursor cursor;
} Console;

void init_console(void);
void clear_console(void);
void console_write_cell(uint8_t colour, char c);

#endif /* __CONSOLE_H__ */
