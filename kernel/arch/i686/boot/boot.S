global _start

extern section_bss_start
extern section_bss_end
extern kernel_stack_base
extern kernel_main
extern hang

section .text

bits 32
_start:
    ; save the multiboot bootloader value
    mov edx, eax

    ; disable interrupts and NMI
    cli
    in al, 0x70
    or al, 0x80
    out 0x70, al

    ; zero out the bss
    cld
    xor al, al
    mov edi, section_bss_start
    mov ecx, section_bss_end
    sub ecx, section_bss_start
    rep stosb

    mov esp, kernel_stack_base
    xor ebp, ebp

    ; TODO: assert that A20 is enabled

    push ebx
    push edx
    call kernel_main

    jmp hang
