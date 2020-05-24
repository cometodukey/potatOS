#include <kernel/serial/serial.h>
#include <kernel/arch/port.h>

static void
serial_configure_baud_rate(SerialPort com, unsigned short divisor) {
  out8(SERIAL_LINE_COMMAND_PORT(com),
       SERIAL_LINE_ENABLE_DLAB);
  out8(SERIAL_DATA_PORT(com),
       (divisor >> 8) & 0x00FF);
  out8(SERIAL_DATA_PORT(com),
       divisor & 0x00FF);
}

void
serial_configure_line(SerialPort com)
{
    /* Bit:     | 7 | 6 | 5 4 3 | 2 | 1 0 |
    ** Content: | d | b | prty  | s | dl  |
    ** Value:   | 0 | 0 | 0 0 0 | 0 | 1 1 | = 0x03
    **/
    out8(SERIAL_LINE_COMMAND_PORT(com), 0x03);
}


void
init_serial(SerialPort com) {
  serial_configure_baud_rate(com, 3);
  serial_configure_line(com);
}

// Checks whether the transmit FIFO queue is empty or not for the given COM
static int
serial_is_transmit_fifo_empty(SerialPort com) {
    /* 0x20 = 0010 0000 */
    return in8(SERIAL_LINE_STATUS_PORT(com)) & 0x20;
}


void
serial_write_character(SerialPort com, char c) {
   while (serial_is_transmit_fifo_empty(com) == 0);

   out8(com, c);
}

void
serial_write_string(SerialPort com, char *str, size_t length) {
  for (size_t i = 0; i < length; i++)
    {
        while (serial_is_transmit_fifo_empty(com) == 0);

        out8(com, str[i]);
    }
}
