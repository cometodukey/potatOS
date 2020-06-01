#include <kernel/types.h>
#include <kernel/events/pic.h>
#include <kernel/lib/panic.h>
#include <kernel/lib/kprintf.h>
#include <kernel/arch/cpu.h>
#include <kernel/events/interrupts.h>
#include <kernel/events/apic.h>

static int apic_support(void);
static void enable_lapic(void);
static uintptr_t get_lapic_base(void);
static void set_lapic_base(uintptr_t apic);
static void lapic_write_reg(uint32_t reg, uint32_t data);
static uint32_t lapic_read_reg(uint32_t reg);

static uintptr_t lapic_base;

void
init_apic(void) {
    kputs("Initialising APIC");

    /* it's necessary to remap all the PIC IRQs as even when it is disabled,
       it could still send spurrious interrupts which will be interpreted as exceptions if the vectors are not remapped */

    /* by default, the IRQs are mapped to vectors 0-15
       this conflicts with Intels reserved exception vectors
       so we need to remap the IRQs to vectors 32-47 */
    remap_pic(0x20, 0x28);
    /* disable the legacy PIC */
    pic_disable();

    if (!apic_support()) {
        PANIC("APIC not supported!");
    }

    enable_lapic();

    enable_nmi();
    sti();
}

static int
apic_support(void) {
    uint32_t eax = 0, ebx = 0, ecx = 0, edx = 0;
    cpuid(1, 0, &eax, &ebx, &ecx, &edx);
    return edx & CPUID_APIC_BIT;
}

static void
enable_lapic(void) {
    lapic_base = get_lapic_base();
    set_lapic_base(lapic_base);

    /* dummy read- apparently avoids a bug in older CPUs */
    lapic_read_reg(0xf0);
    lapic_write_reg(0xf0, lapic_read_reg(0xf0) | 0x1ff);
    kprintf(" LAPIC base is at %p\r\n", lapic_base);
}

static uintptr_t
get_lapic_base(void) {
    uint32_t eax, edx;
    rdmsr(IA32_APIC_BASE_MSR, &eax, &edx);
    return eax & 0xfffff000;
}

static void
set_lapic_base(uintptr_t apic) {
    uint32_t edx = 0;
    uint32_t eax = (apic & 0xfffff000) | IA32_APIC_BASE_MSR_ENABLE;
    wrmsr(IA32_APIC_BASE_MSR, eax, edx);
}

static uint32_t
lapic_read_reg(uint32_t reg) {
    return *(volatile uint32_t *)(lapic_base + reg);
}

static void
lapic_write_reg(uint32_t reg, uint32_t data) {
    *(volatile uint32_t *)(lapic_base + reg) = data;
}
