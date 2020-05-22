#include <kernel/lib/kprint.h>
#include <kernel/types.h>
#include <kernel/arch/idle.h>
#include <kernel/lib/panic.h>
#include <kernel/multiboot.h>
#include <kernel/varargs.h>
#include <kernel/arch/exception.h>
#include <kernel/lib/mem.h>

static void print_registers(const Registers *regs);
static void do_stack_traceback(void);
static char *trace_address(size_t *off, size_t addr);

noreturn void
panic(const char *file, size_t line,
      const char *func, const Registers *regs, const char *msg, ...)
{
    va_list args;
    kprintf("Kernel panic in %s:%u:%s\r\n",
            file, line, func);

    if (regs != NULL) {
        print_registers(regs);
    }
    do_stack_traceback();

    va_start(args, msg);
    kvprintf(msg, args);
    kprint_flush();
    va_end(args);

    hang();
}

// TODO: implement kprintf padding and use it here because register dumps are hard to read
static void
print_registers(const Registers *regs) {
    kprintf("EAX = %x EBX    = %x ECX = %x EDX = %x\r\n", regs->eax, regs->ebx, regs->ecx, regs->edx);
    kprintf("ESI = %x EDI    = %x ESP = %x EBP = %x\r\n", regs->esi, regs->edi, regs->esp, regs->ebp);
    kprintf("EIP = %x EFLAGS = %x\r\n", regs->eip, regs->eflags);
    kprintf("CS  = %x DS     = %x SS  = %x\r\n", regs->cs,  regs->ds, regs->ss);
}

static void
do_stack_traceback(void) {
    StackFrame *stack;
    size_t offset = 0;
    __asm__ volatile("movl %%ebp, %0" : "=r"(stack));
    kputs("Stack trace:");
    for (; stack != NULL; stack = stack->ebp) {
        kprintf(" [%p] %s+%x\r\n",
                stack->eip, trace_address(&offset, (size_t)stack->eip), offset);
    }
}

static char *
trace_address(size_t *off, size_t addr) {
    extern size_t *symlist;
    extern size_t symlist_len;
    size_t i, func_addr;
    char *name;

    if (symlist != NULL) {
        for (i = 0; i < symlist_len ; i++) {
            func_addr = *symlist;
            if (func_addr >= addr) {
                *off = addr - func_addr;
                name = (char *)(symlist + sizeof(size_t) + 1);
                return name;
            }
            for (; *symlist != '\0'; ++i, ++symlist);
        }
    }

    *off = 0;
    return "?";
}
