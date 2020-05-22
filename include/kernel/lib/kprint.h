#ifndef __KPRINT_H__
#define __KPRINT_H__

#include <kernel/varargs.h>
#include <kernel/lib/printf/printf.h>

#define KPRINT_BUF_SIZE (512)

void kprint_flush(void);
void kputchar(char c);
void kputs(const char *s);
void kprint(const char *s);

#endif /* __KPRINT_H__ */
