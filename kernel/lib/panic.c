#include <kernel/lib/kprintf.h>
#include <kernel/types.h>
#include <kernel/arch/arch.h>
#include <kernel/lib/panic.h>
#include <kernel/arch/multiboot.h>
#include <kernel/lib/mem.h>
#include <kernel/lib/symbols.h>

noreturn void
panic(const char *file, size_t line, const char *func,
      const void *regs, const char *msg, ...)
{
    va_list args;
    kprintf("Kernel panic in %s:%u:%s\r\n", file, line, func);
    if (regs != NULL) {
        dump_registers(regs);
    }
    do_stack_trace();
    va_start(args, msg);
    vkprintf((char*)msg, args);
    va_end(args);
    kputs(""); // TODO: write an actual kprint_flush function
    for (;;) {
        hang();
    }
}
