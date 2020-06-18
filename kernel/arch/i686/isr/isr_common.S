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

global pic_pit
global pic_keyboard
global pic_cascade
global pic_com2
global pic_com1
global pic_lpt2
global pic_floppy_disk
global pic_lpt1
global pic_rtc
global pic_unused0
global pic_unused1
global pic_unused2
global pic_ps2_mouse
global pic_coprocessor
global pic_primary_ata
global pic_secondary_ata

extern exception_handler
extern pic_irq
extern pic_eoi

; iretq for 64 bit kernels

%macro EXCEPTION_NO_ERR 1
    cli
    cld
    push 0
    push %1
    jmp exception_common_stub
%endmacro

%macro EXCEPTION_HAS_ERR 1
    cli
    cld
    push %1
    jmp exception_common_stub
%endmacro

%macro LEGACY_PIC_STUB 1
    cli
    pushad
    push %1
    call pic_irq
    push %1
    call pic_eoi
    pop eax
    pop eax
    popad
    iret
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
    ; save registers
    pusha
    push ds
    push es
    push fs
    push gs

    ; load kernel data segment
    mov ax, 0x10
    mov ds, ax
    ; mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; call the handler with a pointer to the registers
    push esp
    call exception_handler
    add esp, 4

    ; restore registers
    pop gs
    pop fs
    pop es
    pop ds
    popa

    ; move stack pointer past error code and interrupt number
    add esp, 8

    ; return from handler
    ; this pops eip, cs, eflags, esp, ss
    iret

; exception handlers
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

; legacy PIC handler stubs
pic_pit:
    LEGACY_PIC_STUB 0
pic_keyboard:
    LEGACY_PIC_STUB 1
pic_cascade: ; should never be raised
    LEGACY_PIC_STUB 2
pic_com2:
    LEGACY_PIC_STUB 3
pic_com1:
    LEGACY_PIC_STUB 4
pic_lpt2:
    LEGACY_PIC_STUB 5
pic_floppy_disk:
    LEGACY_PIC_STUB 6
pic_lpt1:
    LEGACY_PIC_STUB 7
pic_rtc:
    LEGACY_PIC_STUB 8
pic_unused0:
    LEGACY_PIC_STUB 9
pic_unused1:
    LEGACY_PIC_STUB 10
pic_unused2:
    LEGACY_PIC_STUB 11
pic_ps2_mouse:
    LEGACY_PIC_STUB 12
pic_coprocessor:
    LEGACY_PIC_STUB 13
pic_primary_ata:
    LEGACY_PIC_STUB 14
pic_secondary_ata:
    LEGACY_PIC_STUB 15
