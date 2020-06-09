#ifndef __ARCH_H__
#define __ARCH_H__

#include <kernel/types.h>
#include <kernel/arch/multiboot.h>

void init_arch(uint32_t magic, const MultibootInfo *mb);

extern void (*console_put_char)(char);
extern void (*console_put_string)(const char *, size_t);
extern void (*serial_put_char)(char c);
extern void (*serial_put_string)(const char *, size_t);
extern void *(*pmm_alloc)(void);
extern void *(*pmm_zalloc)(void);
extern void (*pmm_free)(void *);
// TODO - paging
extern void (*do_stack_trace)(void);
extern void (*dump_registers)(const void *);
extern void (*idle)(void);
extern void (*hang)(void); // noreturn

#endif /* __ARCH_H__ */
