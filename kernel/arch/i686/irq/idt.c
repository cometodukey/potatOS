#include <kernel/arch/interrupts.h>
#include <kernel/arch/exception.h>
#include <kernel/arch/gdt.h>
#include <kernel/arch/port.h>
#include <kernel/lib/kprint.h>

extern void lidt(uint32_t ptr);
static void idt_entry(uint8_t num, uint32_t base,
                      uint16_t selector, uint8_t type);

static IDTPtr idt_ptr;
static IDTDescriptor idt_entries[256];

void
init_idt(void) {
    size_t i;

    idt_ptr.limit = LEN(idt_entries) * sizeof(IDTDescriptor);
    idt_ptr.base = (uint32_t)&idt_entries;

    /* set all IDT entries */
    for (i = 0; i < LEN(idt_entries); ++i) {
        idt_entry(i, (uint32_t)isr_common_stub, 0, 0x8e);
    }

    /* CPU exception vectors */
    idt_entry(0,  (uint32_t)exception_div0_handler,            8, 0x8e);
    idt_entry(1,  (uint32_t)exception_debug_handler,           8, 0x8e);
    idt_entry(2,  (uint32_t)exception_nmi_handler,             8, 0x8e);
    idt_entry(3,  (uint32_t)exception_breakpoint_handler,      8, 0x8e);
    idt_entry(4,  (uint32_t)exception_overflow_handler,        8, 0x8e);
    idt_entry(5,  (uint32_t)exception_bound_range_handler,     8, 0x8e);
    idt_entry(6,  (uint32_t)exception_invalid_opcode_handler,  8, 0x8e);
    idt_entry(7,  (uint32_t)exception_no_device_handler,       8, 0x8e);
    idt_entry(8,  (uint32_t)exception_double_fault_handler,    8, 0x8e);
    /* co-processor segment overrun (vector 9) is unsupported after i486 */
    idt_entry(9,  (uint32_t)exception_coproc_segment_overrun,  8, 0x8e);
    idt_entry(10, (uint32_t)exception_invalid_tss_handler,     8, 0x8e);
    idt_entry(11, (uint32_t)exception_no_segment_handler,      8, 0x8e);
    idt_entry(12, (uint32_t)exception_ss_fault_handler,        8, 0x8e);
    idt_entry(13, (uint32_t)exception_gpf_handler,             8, 0x8e);
    idt_entry(14, (uint32_t)exception_page_fault_handler,      8, 0x8e);
    /* 15 is reserved */
    idt_entry(16, (uint32_t)exception_x87_fpe_handler,         8, 0x8e);
    idt_entry(17, (uint32_t)exception_alignment_check_handler, 8, 0x8e);
    idt_entry(18, (uint32_t)exception_machine_check_handler,   8, 0x8e);
    idt_entry(19, (uint32_t)exception_simd_fpe_handler,        8, 0x8e);
    idt_entry(20, (uint32_t)exception_virt_handler,            8, 0x8e);
    /* 21 to 29 are reserved */
    idt_entry(30, (uint32_t)exception_security_handler,        8, 0x8e);
    /* 31 is reserved */

    /* load the table */
    lidt((uint32_t)&idt_ptr);

    /* enable interrupts */
    enable_nmi();
    sti();

    kprintf("Loaded IDT at %p\r\n\r\n", &idt_ptr);
}

void
cli(void) {
    __asm__ volatile("cli;");
}

void
sti(void) {
    __asm__ volatile("sti;");
}

void
enable_nmi(void) {
    out8(0x70, in8(0x70) & 0x7F);
}

void
disable_nmi(void) {
    out8(0x70, in8(0x70) | 0x80);
}

static void
idt_entry(uint8_t num, uint32_t base,
          uint16_t selector, uint8_t type)
{
    idt_entries[num].base_low = base & 0xFFFF;
    idt_entries[num].base_high = (base >> 16) & 0xFFFF;

    idt_entries[num].selector = selector;
    idt_entries[num].pad = 0;
    idt_entries[num].type = type;
}
