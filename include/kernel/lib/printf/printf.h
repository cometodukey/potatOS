#ifndef __PRINTF_H__
#define __PRINTF_H__

#include <kernel/types.h>
#include <kernel/varargs.h>


// Printing using kprintf will print to serial port as well when this flag is enabled
#define KPRINTF_PRINT_BOTH 1

ssize_t vkprintf(char *fmt, va_list vlist);
ssize_t kprintf(char *fmt, ...);
ssize_t serial_printf(char *fmt, ...);
ssize_t snprintf(char *dest, ssize_t capacity, char *fmt, ...);
ssize_t vsnprintf(char *dest, ssize_t capacity, char *fmt, va_list vlist);


#endif // __PRINTF_H__
