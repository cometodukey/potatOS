#include <kernel/arch/exception.h>
#include <kernel/lib/kprint.h>
#include <kernel/lib/panic.h>
#include <kernel/lib/assert.h>

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
    "Invalid TSS",
    "No Segment",
    "Stack Fault",
    "GPF",
    "Page Fault",
    "?",
    "x87 Floating Point",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating Point",
    "Virtualisation",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "Security"
};

void
exception_handler(const Registers *regs) {
    // TODO: write a better handler
    //assert(regs->int_no < LEN(names));
    // if (regs->int_no >= LEN(names)) {
    //     regs->int_no = 15;
    // }
    kprintf("Received exception %s (%d) with error code %d\r\n", names[regs->int_no], regs->int_no, regs->error);
    panic(__FILE__, __LINE__, __func__, regs, "interrupt debugging");
}
