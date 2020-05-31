#include <kernel/events/exception.h>
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
    kprintf("Received exception %s (%d) with error code %d\r\n", names[regs->exception], regs->exception, regs->error);
    panic(__FILE__, __LINE__, __func__, regs, "interrupt debugging");
}
