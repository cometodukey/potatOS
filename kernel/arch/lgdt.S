global lgdt

section .text

bits 32
lgdt:
    mov eax, [esp+4]
    lgdt [eax]
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x8:.reload_cs
.reload_cs:
    ret
