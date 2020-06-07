#ifndef __CPU_H__
#define __CPU_H__

#include <kernel/types.h>

#define CPUID_APIC_BIT (1 << 9)

int cpuid(uint32_t leaf, uint32_t subleaf, uint32_t *eax,
          uint32_t *ebx, uint32_t *ecx, uint32_t *edx);
void rdmsr(uint32_t ecx, uint32_t *eax, uint32_t *edx);
void wrmsr(uint32_t ecx, uint32_t eax, uint32_t edx);
uint32_t read_cr2(void);
uint32_t read_cr0(void);
void write_cr0(uint32_t val);
void write_cr3(uint32_t val);

#endif /* __CPU_H__ */
