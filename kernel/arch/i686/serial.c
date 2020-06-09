#include <kernel/arch/arch.h>
#include <kernel/arch/i686/port.h>
#include <kernel/arch/i686/serial.h>

uint16_t com;

static void
serial_configure_baud_rate(uint16_t com, unsigned short divisor) {
    out8(SERIAL_LINE_COMMAND_PORT(com), SERIAL_LINE_ENABLE_DLAB);
    out8(SERIAL_DATA_PORT(com), (divisor >> 8) & 0x00FF);
    out8(SERIAL_DATA_PORT(com), divisor & 0x00FF);
}

static void
serial_configure_line(uint16_t com) {
    /* Bit:     | 7 | 6 | 5 4 3 | 2 | 1 0 |
    ** Content: | d | b | prty  | s | dl  |
    ** Value:   | 0 | 0 | 0 0 0 | 0 | 1 1 | = 0x03
    **/
    out8(SERIAL_LINE_COMMAND_PORT(com), 0x03);
}

void
init_serial(uint16_t init_com) {
    serial_configure_baud_rate(init_com, 3);
    serial_configure_line(init_com);
    com = init_com;
}

/* Checks whether the transmit FIFO queue is empty or not for the given COM */
static int
serial_is_transmit_fifo_empty(uint16_t com) {
    /* 0x20 = 0010 0000 */
    return in8(SERIAL_LINE_STATUS_PORT(com)) & 0x20;
}

void
arch_serial_put_char(char c) {
    while (serial_is_transmit_fifo_empty(com) == 0);

    out8(com, c);
}

void
arch_serial_put_string(const char *str, size_t length) {
    for (size_t i = 0; i < length; i++) {
        while (serial_is_transmit_fifo_empty(com) == 0);

        out8(com, str[i]);
    }
}
