#ifndef __TRACE_H__
#define __TRACE_H__

#include <kernel/types.h>
#include <kernel/attributes.h>
#include <kernel/arch/i686/isr/exception.h>

typedef struct {
    uint32_t ebp;
    uint32_t eip;
} __packed StackFrame;

void arch_dump_registers(const Registers *regs);
void arch_do_stack_trace(void);

#endif /* __TRACE_H__ */
