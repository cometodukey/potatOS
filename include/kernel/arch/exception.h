#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__

#include <kernel/types.h>

void isr_common_stub(void);
void exception_div0_handler(void);
void exception_debug_handler(void);
void exception_nmi_handler(void);
void exception_breakpoint_handler(void);
void exception_overflow_handler(void);
void exception_bound_range_handler(void);
void exception_invalid_opcode_handler(void);
void exception_no_device_handler(void);
void exception_double_fault_handler(void);
void exception_invalid_tss_handler(void);
void exception_no_segment_handler(void);
void exception_ss_fault_handler(void);
void exception_gpf_handler(void);
void exception_page_fault_handler(void);
void exception_x87_fpe_handler(void);
void exception_alignment_check_handler(void);
void exception_machine_check_handler(void);
void exception_simd_fpe_handler(void);
void exception_virt_handler(void);
void exception_security_handler(void);

enum {
    DIV0 = 0,
    DEBUG,
    NMI,
    BREAKPOINT,
    OVERFLOW,
    BOUND_RANGE,
    INVALID_OPCODE,
    NO_DEVICE,
    DOUBLE_FAULT,
    INVALID_TSS,
    NO_SEGMENT,
    STACK_FAULT,
    GPF,
    PAGE_FAULT,
    X87_FPE,
    ALIGNMENT_CHECK,
    MACHINE_CHECK,
    SIMD_FPE,
    VIRT,
    SECURITY
};

typedef struct {
    size_t ds;
    size_t edi, esi, ebp, unused, ebx, edx, ecx, eax;
    size_t int_no, error;
    size_t eip, cs, eflags, esp, ss;
} __attribute__((packed)) Registers;

#endif /* __EXCEPTION_H__ */
