#include <kernel/types.h>
#include <kernel/arch/i686/cpu.h>
#include <kernel/arch/i686/mm/paging.h>
#include <kernel/lib/errno.h>
#include <kernel/lib/assert.h>
#include <kernel/arch/i686/mm/pmm.h>
#include <kernel/lib/kprintf.h>
#include <kernel/lib/panic.h>
#include <kernel/attributes.h>
#include <kernel/lib/mem.h>

// TODO - i may need global pages at some point so remember to come back and enable them
//        set CR4.SMEP and CR4.SMAP?
//        set IA32_EFER.NXE
//        check for paging support - see Intel SDM Volume 3 Chapter 4.1.4
//        PAE support

static int has_pae_support(void);
static void invlpg(uintptr_t addr);

static uint32_t __align(PAGE_SIZE) kernel_page_directory[1024];

void
init_paging(void) {
    size_t i;
    uintptr_t addr;

    if (!has_pae_support()) {
        PANIC("PAE is not supported!");
    }

    /* NULL each entry - this forces physical pages to be mapped later on */
    for (i = 0; i < LEN(kernel_page_directory); ++i) {
        kernel_page_directory[i] = 0;
    }

    /* identity map the first 4MiB */
    // for (i = 0; i < LEN(first_mib_page_table); ++i) {
    //     first_mib_page_table[i] = (i * PAGE_SIZE) | (PAGE_RDWR | PAGE_PRESENT);
    // }
    /* make the first entry present, readable and writeable */
    // kernel_page_directory[0] = (uint32_t)first_mib_page_table | (PAGE_RDWR | PAGE_PRESENT);

    /* identity map the first MiB */
    for (i = 1; i < 1024; ++i) {
        addr = i * PAGE_SIZE;
        arch_map_page(kernel_page_directory, addr, addr, PAGE_RDWR | PAGE_PRESENT);
    }
    /* map the zero page and make it not present */
    arch_map_page(kernel_page_directory, (uintptr_t)NULL, (uintptr_t)NULL, 0);

    kernel_page_directory[0] |= (PAGE_RDWR | PAGE_PRESENT);

    /* point CR3 to the kernels page directory */
    write_cr3((uint32_t)kernel_page_directory);

    /* enable paging and write protect read-only supervisor pages */
    write_cr0(read_cr0() | CR0_PG | CR0_WP);

    // TODO
    // identity map the first MiB
    // mark the first MiB as not present
    // map the pages under the kernel somewhere else
    // map the pages under multiboot modules somewhere else
    // set up a new stack with guard page
    // make text and rodata read only
    // identity map video buffer

    /* PAE, 4KB pages, no global pages */
    //write_cr4((read_cr4() | CR4_PAE) & ~(CR4_PSE | CR4_PGE));
}

static int
has_pae_support(void) {
    uint32_t eax, ebx, ecx, edx;
    cpuid(0, 1, &eax, &ebx, &ecx, &edx);
    return edx & CPUID_PAE_BIT;
}

static void
invlpg(uintptr_t addr) {
    __asm__ volatile ("invlpg (%0);"
                      : : "r" (addr)
                      : "memory");
}

KernelResult
arch_map_page(uint32_t pd[1024], uintptr_t phys_addr, uintptr_t virt_addr, int flags) {
    void *tmp;
    int pmm_used = 0;
    uint32_t *pt;

    /* find the table indices using the physical address */
    int pd_entry = virt_addr >> 22;
    int pt_entry = (virt_addr >> 12) & 0x3FF;

    assert(!((int)pd % PAGE_SIZE));
    assert(!(phys_addr % PAGE_SIZE));
    assert(!(virt_addr % PAGE_SIZE));

    if ((void *)(pd[pd_entry] & PAGE_MASK) == NULL) {
        if ((void *)(tmp = arch_pmm_zalloc()) == NULL) {
            return GENERIC_ERR;
        }
        pd[pd_entry] |= (uint32_t)tmp;
        pmm_used = 1;
    }
    pt = (void *)(pd[pd_entry] & PAGE_MASK);
    if ((void *)(pt[pt_entry] & PAGE_MASK) != NULL) {
        if (pmm_used) {
            arch_pmm_free(tmp);
            pd[pd_entry] &= ~PAGE_MASK;
        }
        return GENERIC_ERR;
    }
    pt[pt_entry] |= (phys_addr | flags);
    /* reload CR3 if the page directory is loaded */
    if (read_cr3() == (uint32_t)pd) {
        // TODO - TLB shootdown
        invlpg(virt_addr);
    }
    return GENERIC_SUCCESS;
}

KernelResult
arch_unmap_page() {
    return GENERIC_ERR;
}

KernelResult
arch_remap_page() {
    return GENERIC_ERR;
}
