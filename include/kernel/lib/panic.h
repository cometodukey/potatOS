#ifndef __PANIC_H__
#define __PANIC_H__

#include <kernel/types.h>
#include <kernel/attributes.h>

#define PANIC(...) \
    panic(__FILE__, __LINE__, __func__, NULL, __VA_ARGS__)

noreturn void
panic(const char *file, size_t line,
      const char *func, const void *regs, const char *msg, ...);

#endif /* __PANIC_H__ */
