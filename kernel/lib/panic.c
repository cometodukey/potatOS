#include <kernel/lib/kprint.h>
#include <kernel/types.h>
#include <kernel/arch/idle.h>
#include <kernel/lib/panic.h>
#include <kernel/multiboot.h>
#include <kernel/varargs.h>
#include <kernel/arch/exception.h>
#include <kernel/lib/mem.h>
#include <kernel/lib/symbols.h>

static void print_registers(const Registers *regs);
static void do_stack_traceback(void);
static char *trace_address(size_t *off, size_t addr);

static char name[SYM_NAME_SIZ];

noreturn void
panic(const char *file, size_t line, const char *func,
      const Registers *regs, const char *msg, ...)
{
    va_list args;
    kprintf("Kernel panic in %s:%u:%s\r\n", file, line, func);
    if (regs != NULL) {
        print_registers(regs);
    }
    do_stack_traceback();

    va_start(args, msg);
    vkprintf((char*)msg, args);
    kprint_flush();
    va_end(args);

    hang();
}

static void
print_registers(const Registers *regs) {
    kprintf("EAX = %08x EBX    = %08x ECX = %08x EDX = %08x\r\n", regs->eax, regs->ebx, regs->ecx, regs->edx);
    kprintf("ESI = %08x EDI    = %08x ESP = %08x EBP = %08x\r\n", regs->esi, regs->edi, regs->esp, regs->ebp);
    kprintf("EIP = %08x EFLAGS = %08x\r\n", regs->eip, regs->eflags);
    kprintf("CS  = %08x DS     = %08x SS  = %08x\r\n", regs->cs,  regs->ds, regs->ss);
}

static void
do_stack_traceback(void) {
    StackFrame *stack;
    size_t offset = 0;
    __asm__ volatile("movl %%ebp, %0" : "=r" (stack));
    kputs("Stack trace:");
    for (; stack != NULL; stack = stack->ebp) {
        kprintf(" [%p] %s+%x\r\n",
                stack->eip, trace_address(&offset, (size_t)stack->eip), offset);
    }
}

static char *
trace_address(size_t *off, size_t addr) {
    extern MultibootModule *kernel_syms;
    if (kernel_syms == NULL) {
        *off = 0;
        return "?";
    }
    size_t base = parse_symlist(kernel_syms, name, addr);
    *off = addr - base;
    return name;
}
