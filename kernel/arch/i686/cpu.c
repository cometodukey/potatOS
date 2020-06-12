#include <kernel/types.h>
#include <kernel/arch/arch.h>

void
init_cpu(void) {
    // TODO - setup CPU features
}

// TODO: check this is correct for IA-32
int
cpuid(uint32_t leaf, uint32_t subleaf, uint32_t *eax,
      uint32_t *ebx, uint32_t *ecx, uint32_t *edx)
{
    uint32_t cpuid_max;
    __asm__ volatile ("cpuid;"
                      : "=a" (cpuid_max)
                      : "a" (leaf & 0x80000000) : "ebx", "ecx", "edx");
    if (leaf > cpuid_max)
        return 0;
    __asm__ volatile ("cpuid;"
                      : "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx)
                      : "a" (leaf), "c" (subleaf));
    return 1;
}

/* FIXME: using these is janky - use uint64_t? */
void
rdmsr(uint32_t ecx, uint32_t *eax, uint32_t *edx) {
    __asm__ volatile ("rdmsr;"
                      : "=a" (*eax), "=d" (*edx)
                      : "c" (ecx));
}

void
wrmsr(uint32_t ecx, uint32_t eax, uint32_t edx) {
    __asm__ volatile ("wrmsr;"
                      :
                      : "a" (eax), "d" (edx), "c" (ecx));
}

uint32_t
read_cr2(void) {
    uint32_t cr2;
    __asm__ volatile ("movl %%cr2, %0;"
                      : "=r" (cr2));
    return cr2;
}

uint32_t
read_cr0(void) {
    uint32_t cr0;
    __asm__ volatile ("movl %%cr0, %0;"
                      : "=r" (cr0));
    return cr0;
}

void
write_cr0(uint32_t val) {
    __asm__ volatile ("movl %0, %%cr0;"
                      :
                      : "r" (val));
}

uint32_t
read_cr3(void) {
    uint32_t cr3;
    __asm__ volatile ("movl %%cr3, %0;"
                      : "=r" (cr3));
    return cr3;
}

void
write_cr3(uint32_t val) {
    __asm__ volatile ("movl %0, %%cr3;"
                      :
                      : "r" (val));
}

uint32_t
read_cr4(void) {
    uint32_t cr4;
    __asm__ volatile ("movl %%cr4, %0;"
                      : "=r" (cr4));
    return cr4;
}

void
write_cr4(uint32_t val) {
    __asm__ volatile ("movl %0, %%cr4;"
                      :
                      : "r" (val));
}
