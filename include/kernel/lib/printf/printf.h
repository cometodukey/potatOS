#ifndef __PRINTF_H__
#define __PRINTF_H__

#include <kernel/types.h>
#include <kernel/varargs.h>

ssize_t vkprintf(char *fmt, va_list vlist);
ssize_t kprintf(char *fmt, ...);
ssize_t snprintf(char *dest, ssize_t capacity, char *fmt, ...);
ssize_t vsnprintf(char *dest, ssize_t capacity, char *fmt, va_list vlist);


#endif // __PRINTF_H__
