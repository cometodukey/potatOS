#include <kernel/types.h>
#include <kernel/events/pic.h>
#include <kernel/lib/panic.h>
#include <kernel/lib/kprint.h>
#include <kernel/arch/cpu.h>
#include <kernel/events/interrupts.h>

static int apic_support(void);

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

    enable_nmi();
    sti();
}

static int
apic_support(void) {
    uint32_t eax, ebx, ecx, edx = 0;

    cpuid(1, 0, &eax, &ebx, &ecx, &edx);
    return edx & CPUID_APIC_BIT;
}
