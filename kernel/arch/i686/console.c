#include <kernel/arch/i686/console.h>
#include <kernel/lib/mem.h>
#include <kernel/arch/i686/port.h>

static void console_scroll(void);
static void disable_hw_cursor(void);

static Console console = {
    .addr = (uint16_t *)0xb8000,
    .bpp = 16,
    .pitch = 160,
    .cols = 80,
    .rows = 25,
    .cursor = {
        .col = 0,
        .row = 0
    }
};

void
init_console(void) {
    // TODO: set console from multiboot info
    disable_hw_cursor();
    clear_console();
}

void
clear_console(void) {
    size_t i, j;
    console.cursor.col = console.cursor.row = 0;
    for (i = 0; i < console.cols; ++i) {
        for (j = 0; j < console.rows; ++j) {
            arch_console_put_char(' ');
        }
    }
    console.cursor.col = console.cursor.row = 0;
}

static void
disable_hw_cursor(void) {
	out8(0x3D4, 0x0A);
	out8(0x3D5, 0x20);
}

void
arch_console_put_string(const char *string, size_t len) {
    size_t i;
    for (i = 0; i < len; ++i) {
        arch_console_put_char(string[i]);
    }
}

void
arch_console_put_char(char c) {
    switch (c) {
    case '\n':
        console.cursor.row++;
        break;
    case '\r':
        console.cursor.col = 0;
        break;
    case '\t':
        // TODO: clear chars under tab
        console.cursor.col += CONSOLE_TAB_WIDTH;
        break;
    case '\b':
        // TODO: clear char under backspace
        console.cursor.col--;
        break;
    default:
        console.addr[console.cursor.row*80 + console.cursor.col] = (MAKE_COLOUR(WHITE, BLACK) << 8) | c;
        console.cursor.col++;
    }

    if (console.cursor.col > console.cols - 1) {
        console.cursor.col = 0;
        console.cursor.row ++;
    }

    if (console.cursor.row >= console.rows) {
        console_scroll();
        console.cursor.row = console.rows - 1;
    }
}

static void
console_scroll(void) {
    size_t i;
    for (i = 0*80; i < 24*80; i++) {
        console.addr[i] = console.addr[i+80];
    }

    // memmove(&console.addr[CONSOLE_ROW(0)],
    //         &console.addr[CONSOLE_ROW(1)],
    //         CONSOLE_BUF_SIZE - console.pitch * 1);

    /* clear the last line */
    for (i = 24*80; i < 25*80; i++) {
        console.addr[i] = (MAKE_COLOUR(BLACK, BLACK) << 8) | ' ';
    }
}

// TODO: cleanup these functions and remove the magic numbers and also fix the bug where the last row isn't used

// void
// console_write_cell(uint8_t colour, char c) {
//     switch (c) {
//     case '\n':
//         console.cursor.row++;
//         return;
//     case '\r':
//         console.cursor.col = 0;
//         return;
//     case '\t':
//         // TODO: clear characters under tab
//         console.cursor.col += CONSOLE_TAB_WIDTH;
//         return;
//     }

//     console.addr[CONSOLE_CURSOR_POS] = (colour << 8) | c;

//     if (++console.cursor.col > console.cols-1) {
//         console.cursor.col = 0;
//         console.cursor.row++;
//     }
//     if (console.cursor.row >= console.rows) {
//         console_scroll();
//         console.cursor.row = console.rows-1;
//     }
// }

// static void
// console_scroll(void) {
//     size_t col;
//     /* copy all but the first row up */
//     memmove(&console.addr[CONSOLE_ROW(0)],
//             &console.addr[CONSOLE_ROW(1)],
//             CONSOLE_BUF_SIZE);// - console.pitch * 1);
//     /* clear the last row */
//     for (col = 0; col < console.cols; ++col) {
//         console.addr[CONSOLE_ROW(console.rows-1)+col] = (MAKE_COLOUR(BLACK, RED) << 8) | 'z';
//     }
// }
