#include <kernel/types.h>
#include <kernel/arch/pic.h>
#include <kernel/lib/panic.h>
#include <kernel/lib/kprint.h>
#include <kernel/arch/cpu.h>
#include <kernel/arch/interrupts.h>

static int apic_support(void);

void
init_apic(void) {
    /* by default, the IRQs are mapped to vectors 0-15
       this conflicts with Intels reserved exception vectors
       so we need to remap the IRQs to vectors 32-47 */
    remap_pic(0x20, 0x28);
    /* disable the legacy PIC */ /* FIXME: do i need to remap the vectors? */
    pic_disable();

    kprint("Is APIC supported... ");
    if (!apic_support()) {
        kputs("No");
        PANIC("APIC not supported!");
    }
    kputs("Yes");

    enable_nmi();
    sti();
}

static int
apic_support(void) {
    uint32_t eax, ebx, ecx, edx = 0;

    cpuid(1, 0, &eax, &ebx, &ecx, &edx);
    return edx & CPUID_APIC_BIT;
}
