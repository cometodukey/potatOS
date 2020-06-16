#include <kernel/arch/i686/isr/exception.h>
#include <kernel/arch/i686/cpu.h>
#include <kernel/lib/kprintf.h>
#include <kernel/lib/panic.h>
#include <kernel/lib/assert.h>
#include <kernel/arch/arch.h>

/* '?' are reserved exceptions */
static const char *names[] = {
    "Division by 0",
    "Debug",
    "NMI",
    "Breakpoint",
    "Overflow",
    "Bound Range",
    "Invalid Opcode",
    "No Device",
    "Double Fault",
    "Co-processor segment overrun",
    "Invalid TSS",
    "No Segment",
    "Stack Fault",
    "GPF",
    "Page Fault",
    "?", /* 15 */
    "x87 Floating Point",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating Point",
    "Virtualisation",
    "?", /* 21 */
    "?", /* 22 */
    "?", /* 23 */
    "?", /* 24 */
    "?", /* 25 */
    "?", /* 26 */
    "?", /* 27 */
    "?", /* 28 */
    "?", /* 29 */
    "Security",
    "?" /* 30 */
};

void
exception_handler(const Registers *regs) {
    assert(regs->exception < LEN(names));
    // TODO - save CR2 on PF
    if (regs->exception == PAGE_FAULT) {
        kprintf("CR2 = %08X\r\n", read_cr2());
    }
    kprintf("Received exception %s (%d) with error code %d. Faulting EIP %08X\r\n", names[regs->exception], regs->exception, regs->error, regs->eip);
    // dump_registers(regs);
    do_stack_trace();
}
