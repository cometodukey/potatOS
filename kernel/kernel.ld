ENTRY(_start)

SECTIONS {
    . = 1M;

    .text ALIGN(4K) : {
        KEEP(*(.boot));
        *(.text);
        *(.text.*);
    }

    .rodata ALIGN(4K) : {
        *(.rodata);
        *(.rodata.*);
    }

    .data ALIGN(4K) : {
        *(.data);
        *(.data.*);
    }

    .bss ALIGN(4K) : {
        section_bss_start = .;
        *(.bss);
        *(.bss.*);
        section_bss_end = .;
    }
}
