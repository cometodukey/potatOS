#ifndef __CPU_H__
#define __CPU_H__

#include <kernel/types.h>

#define CPUID_APIC_BIT (1 << 9)

enum CR0_BITS {
    CR0_PE = 1 << 0,  /* enable protected mode */
    CR0_MP = 1 << 1,
    CR0_EM = 1 << 2,
    CR0_TS = 1 << 3,
    CR0_ES = 1 << 4,
    CR0_NE = 1 << 5,
    CR0_WP = 1 << 16, /* write protected supervisor pages */
    CR0_AM = 1 << 18,
    CR0_NW = 1 << 29,
    CR0_CD = 1 << 30,
    /* C11 doesn't allow enumerator values outside the range of `int` */
#define CR0_PG (1 << 31) /* enable paging */
};

enum CR4_BITS {
    CR4_VME        = 1 << 0,
    CR4_PVI        = 1 << 1,
    CR4_TSD        = 1 << 2,
    CR4_DE         = 1 << 3,
    CR4_PSE        = 1 << 4,
    CR4_PAE        = 1 << 5, /* enable PAE */
    CR4_MCE        = 1 << 6,
    CR4_PGE        = 1 << 7,
    CR4_PCE        = 1 << 8,
    CR4_OSFXR      = 1 << 9,
    CR4_OSXMMEXCPT = 1 << 10,
    CR4_UMIP       = 1 << 11,
    CR4_LA57       = 1 << 12,
    CR4_VMXE       = 1 << 13,
    CR4_SMXE       = 1 << 14,
    CR4_FSGSBASE   = 1 << 16,
    CR4_PCIDE      = 1 << 17,
    CR4_OSXSAVE    = 1 << 18,
    CR4_SMEP       = 1 << 20,
    CR4_SMAP       = 1 << 21,
    CR4_PKE        = 1 << 22
};

int cpuid(uint32_t leaf, uint32_t subleaf, uint32_t *eax,
          uint32_t *ebx, uint32_t *ecx, uint32_t *edx);
void rdmsr(uint32_t ecx, uint32_t *eax, uint32_t *edx);
void wrmsr(uint32_t ecx, uint32_t eax, uint32_t edx);
uint32_t read_cr2(void);
uint32_t read_cr0(void);
void write_cr0(uint32_t val);
void write_cr3(uint32_t val);
uint32_t read_cr4(void);
void write_cr4(uint32_t val);

#endif /* __CPU_H__ */
