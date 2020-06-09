#include <kernel/lib/kprintf.h>
#include <kernel/lib/symbols.h>
#include <kernel/arch/multiboot.h>
#include <kernel/arch/i686/isr/exception.h>
#include <kernel/arch/i686/trace.h>

static char name[SYM_NAME_SIZ];

void
arch_dump_registers(const Registers *regs) {
    kprintf("EAX = %08x EBX    = %08x ECX = %08x EDX = %08x\r\n", regs->eax, regs->ebx, regs->ecx, regs->edx);
    kprintf("ESI = %08x EDI    = %08x ESP = %08x EBP = %08x\r\n", regs->esi, regs->edi, regs->esp, regs->ebp);
    kprintf("EIP = %08x EFLAGS = %08x\r\n", regs->eip, regs->eflags);
    kprintf("CS  = %8.4x DS     = %8.4x SS  = %8.4x\r\n", regs->cs,  regs->ds, regs->ss);
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

void
arch_do_stack_trace(void) {
    StackFrame *stack;
    size_t offset = 0;
    __asm__ volatile("movl %%ebp, %0" : "=r" (stack));
    kputs("Stack trace:");
    for (; stack != NULL; stack = (StackFrame *)stack->ebp) {
        kprintf(" [%.8p] %s+%x\r\n",
                stack->eip, trace_address(&offset, (size_t)stack->eip), offset);
    }
}
