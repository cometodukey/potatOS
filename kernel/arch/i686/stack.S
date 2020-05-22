global kernel_stack_base
global kernel_stack_top

section .bss

kernel_stack_top:
    resb 4096 ; 4KB stack
align 16
kernel_stack_base:
