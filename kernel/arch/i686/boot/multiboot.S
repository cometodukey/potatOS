%define MB_MAGIC            (0x1BADB002)
%define MB_FLAGS_PAGE_BOUND (1 << 0)
%define MB_FLAGS_MMAP       (1 << 1)
%define MB_FLAGS_VIDEO      (1 << 2)
%define MB_VIDEO_EGA_TEXT    1

%define FLAGS (MB_FLAGS_PAGE_BOUND | MB_FLAGS_VIDEO | MB_FLAGS_MMAP)

global multiboot_header

section .boot

align 4
multiboot_header:
    .magic:     dd MB_MAGIC
    .flags:     dd FLAGS
    .checksum:  dd -(MB_MAGIC + FLAGS)
    ; unused parameters
    .unused:    dd 0, 0, 0, 0, 0
    ; framebuffer
    .mode_type: dd MB_VIDEO_EGA_TEXT
    .width:     dd 0
    .height:    dd 0
    .depth:     dd 0
