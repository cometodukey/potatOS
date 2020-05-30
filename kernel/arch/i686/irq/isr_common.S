global lidt
global isr_common_stub
global exception_common_stub
global exception_div0_handler
global exception_debug_handler
global exception_nmi_handler
global exception_breakpoint_handler
global exception_overflow_handler
global exception_bound_range_handler
global exception_invalid_opcode_handler
global exception_no_device_handler
global exception_double_fault_handler
global exception_coproc_segment_overrun
global exception_invalid_tss_handler
global exception_no_segment_handler
global exception_ss_fault_handler
global exception_gpf_handler
global exception_page_fault_handler
global exception_x87_fpe_handler
global exception_alignment_check_handler
global exception_machine_check_handler
global exception_simd_fpe_handler
global exception_virt_handler
global exception_security_handler

extern exception_handler

%define GDT_KERNEL_DS 0x10

; iretq for 64 bit kernels

%macro EXCEPTION_NO_ERR 1
    cli
    push 0
    push %1
    jmp exception_common_stub
%endmacro

%macro EXCEPTION_HAS_ERR 1
    cli
    push %1
    jmp exception_common_stub
%endmacro

section .text

bits 32

lidt:
    mov eax, dword [esp+4]
    lidt [eax]
    ret

isr_common_stub:
    iret

exception_common_stub:
    ; exception, error code, eip, esp, cs, ss, eflags, es, gs, fs
    ; save registers
    pushad
    cld

    ; load the kernel data segment
    xor eax, eax
    mov ax, ds
    push eax
    mov ax, GDT_KERNEL_DS
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; pass a pointer to the registers
    push esp
    call exception_handler

    pop eax

    ; restore the previous data segment
    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; restore registers and return
    popad
    iret

; TODO: add legacy coprocessor segment overrun handler
exception_div0_handler:
    EXCEPTION_NO_ERR 0
exception_debug_handler:
    EXCEPTION_NO_ERR 1
exception_nmi_handler:
    EXCEPTION_NO_ERR 2
exception_breakpoint_handler:
    EXCEPTION_NO_ERR 3
exception_overflow_handler:
    EXCEPTION_NO_ERR 4
exception_bound_range_handler:
    EXCEPTION_NO_ERR 5
exception_invalid_opcode_handler:
    EXCEPTION_NO_ERR 6
exception_no_device_handler:
    EXCEPTION_NO_ERR 7
exception_double_fault_handler:
    EXCEPTION_HAS_ERR 8
exception_coproc_segment_overrun:
    EXCEPTION_HAS_ERR 9
exception_invalid_tss_handler:
    EXCEPTION_HAS_ERR 10
exception_no_segment_handler:
    EXCEPTION_HAS_ERR 11
exception_ss_fault_handler:
    EXCEPTION_HAS_ERR 12
exception_gpf_handler:
    EXCEPTION_HAS_ERR 13
exception_page_fault_handler:
    EXCEPTION_HAS_ERR 14
; 15 is reserved
exception_x87_fpe_handler:
    EXCEPTION_NO_ERR 16
exception_alignment_check_handler:
    EXCEPTION_HAS_ERR 17
exception_machine_check_handler:
    EXCEPTION_NO_ERR 18
exception_simd_fpe_handler:
    EXCEPTION_NO_ERR 19
exception_virt_handler:
    EXCEPTION_NO_ERR 20
; 21 to 29 are reserved
exception_security_handler:
    EXCEPTION_HAS_ERR 30
; 31 is reserved
