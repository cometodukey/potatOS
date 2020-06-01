#ifndef __APIC_H__
#define __APIC_H__

#define IA32_APIC_BASE_MSR 0x1B
#define IA32_APIC_BASE_MSR_ENABLE 0x800

// TODO: add the other APIC registers
enum {
    APIC_ID      = 0x20,
    APIC_VERSION = 0x30,
    APIC_TPR     = 0x80,
    APIC_APR     = 0x90,
    APIC_PPR     = 0xA0,
    APIC_EOI     = 0xB0,
    APIC_RRD     = 0xC0,
    APIC_LDR     = 0xD0,
    APIC_DFR     = 0xE0,
    APIC_SIV     = 0xF0
};

void init_apic(void);

#endif /* __APIC_H__ */
