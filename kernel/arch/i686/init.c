#include <kernel/arch/multiboot.h>
#include <kernel/arch/i686/console.h>
#include <kernel/arch/i686/serial.h>
#include <kernel/arch/i686/mm/pmm.h>
#include <kernel/arch/i686/mm/paging.h>
#include <kernel/arch/i686/trace.h>
#include <kernel/arch/i686/idle.h>
#include <kernel/arch/i686/tables/gdt.h>
#include <kernel/arch/i686/isr/interrupts.h>
#include <kernel/arch/i686/cpu.h>

void (*console_put_char)(char);
void (*console_put_string)(const char *, size_t);
void (*serial_put_char)(char c);
void (*serial_put_string)(const char *, size_t);
void *(*pmm_alloc)(void);
void *(*pmm_zalloc)(void);
void (*pmm_free)(void *);
// TODO - paging
void (*do_stack_trace)(void);
void (*dump_registers)(const void *);
void (*idle)(void);
void (*hang)(void); // noreturn

void
init_arch(uint32_t magic, const MultibootInfo *mb) {
    console_put_char   = arch_console_put_char;
    console_put_string = arch_console_put_string;
    serial_put_char    = arch_serial_put_char;
    serial_put_string  = arch_serial_put_string;
    pmm_alloc          = arch_pmm_alloc;
    pmm_zalloc         = arch_pmm_zalloc;
    pmm_free           = arch_pmm_free;
    do_stack_trace     = arch_do_stack_trace;
    dump_registers     = (void (*)(const void *))arch_dump_registers;
    idle               = arch_idle;
    hang               = arch_hang;

    /* load descriptor tables */
    init_gdt();
    init_idt();

    /* zero CR3 */
    write_cr3((uintptr_t)NULL);

    /* set up CPU features */
    init_cpu();

    init_serial(SERIAL_COM1_BASE);
    init_console();

    /* enumerate module list, find kernel command line, etc */
    parse_multiboot_info(magic, mb);

    /* initialise memory systems */
    init_pmm(mb->mmap_addr, mb->mmap_length);
    init_paging();
    // TODO - set up a new stack and guard page after paging is initialised
}
