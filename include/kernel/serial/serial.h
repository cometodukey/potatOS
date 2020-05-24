#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <kernel/types.h>

#define SERIAL_COM1_BASE                0x3F8      /* COM1 base port */

#define SERIAL_DATA_PORT(base)          (base)
#define SERIAL_FIFO_COMMAND_PORT(base)  (base + 2)
#define SERIAL_LINE_COMMAND_PORT(base)  (base + 3)
#define SERIAL_MODEM_COMMAND_PORT(base) (base + 4)
#define SERIAL_LINE_STATUS_PORT(base)   (base + 5)

#define SERIAL_LINE_ENABLE_DLAB         0x80


typedef unsigned int SerialPort;

void init_serial(SerialPort com);
void serial_write_character(SerialPort com, char a);
void serial_write_string(SerialPort com, char *str, size_t length);
#endif // __SERIAL_H__
