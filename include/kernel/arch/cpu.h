#ifndef __CPU_H__
#define __CPU_H__

#define CPUID_APIC_BIT (1 << 9)

int cpuid(uint32_t leaf, uint32_t subleaf, uint32_t *eax, 
          uint32_t *ebx, uint32_t *ecx, uint32_t *edx);

#endif /* __CPU_H__ */
