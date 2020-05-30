#include <kernel/types.h>

int
cpuid(uint32_t leaf, uint32_t subleaf, uint32_t *eax,
      uint32_t *ebx, uint32_t *ecx, uint32_t *edx)
{
    uint32_t cpuid_max;
    __asm__ volatile ("cpuid"
                      : "=a" (cpuid_max)
                      : "a" (leaf & 0x80000000) : "rbx", "rcx", "rdx");
    if (leaf > cpuid_max)
        return 0;
    __asm__ volatile ("cpuid"
                      : "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx)
                      : "a" (leaf), "c" (subleaf));
    return 1;
}
