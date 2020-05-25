#ifndef __PANIC_H__
#define __PANIC_H__

#include <kernel/types.h>
#include <kernel/multiboot.h>
#include <kernel/arch/exception.h>

#define PANIC(...) \
    panic(__FILE__, __LINE__, __func__, NULL, __VA_ARGS__)

typedef  struct __attribute__((packed)) StackFrame {
    struct StackFrame *ebp;
    uint32_t *eip;
} StackFrame;

noreturn void
panic(const char *file, size_t line,
      const char *func, const Registers *regs, const char *msg, ...);

#endif /* __PANIC_H__ */
