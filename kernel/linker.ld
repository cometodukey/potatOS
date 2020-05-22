ENTRY(_start)

SECTIONS {
    . = 1M;

    .text ALIGN(4K) : {
        KEEP(*(.multiboot))
        KEEP(*(.text))
    }

    .rodata ALIGN(4K) : {
        KEEP(*(.rodata))
    }

    .data ALIGN(4K) : {
        KEEP(*(.data))
    }

    .bss ALIGN(4K) : {
        section_bss_start = .;
        KEEP(*(.bss))
        KEEP(*(COMMON))
        section_bss_end = .;
    }
}
