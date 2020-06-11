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

static uint32_t __align(PAGE_SIZE) kernel_page_directory[1024];
static uint32_t __align(PAGE_SIZE) first_mib_page_table[1024];

void
init_paging(void) {
    size_t i;

    if (!has_pae_support()) {
        PANIC("PAE is not supported!");
    }

    /* mark each page table as not present */
    for (i = 0; i < LEN(kernel_page_directory); ++i) {
        kernel_page_directory[i] = !PAGE_PRESENT;
    }

    /* identity map the first 4MiB */
    for (i = 0; i < LEN(first_mib_page_table); ++i) {
        first_mib_page_table[i] = (i * PAGE_SIZE) | (PAGE_RDWR | PAGE_PRESENT);
    }
    /* make the first entry present, readable and writeable */
    kernel_page_directory[0] = (uint32_t)first_mib_page_table | (PAGE_RDWR | PAGE_PRESENT);

    /* point CR3 to the kernels page directory */
    write_cr3((uint32_t)kernel_page_directory);

    /* enable paging and write protect read-only supervisor pages */
    write_cr0(read_cr0() | CR0_PG | CR0_WP);

    arch_map_page(kernel_page_directory, (uintptr_t)NULL,            (uintptr_t)NULL, 0);
    arch_map_page(kernel_page_directory, (uintptr_t)PAGE_SIZE,       (uintptr_t)NULL, 0);
    // arch_map_page(kernel_page_directory, (uintptr_t)MEM_BASE * 4,    (uintptr_t)NULL, 0);
    // arch_map_page(kernel_page_directory, (uintptr_t)UINT32_MAX-4096, (uintptr_t)NULL, 0);
    // arch_map_page(kernel_page_directory, (uintptr_t)UINT32_MAX,      (uintptr_t)NULL, 0);

    /* PAE, 4KB pages, no global pages */
    //write_cr4((read_cr4() | CR4_PAE) & ~(CR4_PSE | CR4_PGE));
}

static int
has_pae_support(void) {
    uint32_t eax, ebx, ecx, edx;
    cpuid(0, 1, &eax, &ebx, &ecx, &edx);
    return edx & CPUID_PAE_BIT;
}

KernelResult
arch_map_page(uint32_t pd[1024], uintptr_t phys_addr, uintptr_t virt_addr, int flags) {
    uint32_t *pt, *tmp;
    int pmm_used = 0;
    /* find the page directory and page table offset */
    uint32_t pd_entry = phys_addr / (MEM_BASE * 4);
    uint32_t pt_entry = (phys_addr / PAGE_SIZE) % 1024;

    kprintf("addr = %.8p, pd_entry = %u, pt_entry = %u\r\n", phys_addr, pd_entry, pt_entry);

    /* allocate a new table if one does not exist already */
    if ((pd[pd_entry] & PAGE_MASK) == (uint32_t)NULL) {
        tmp = arch_pmm_zalloc();
        if (tmp == NULL) {
            return GENERIC_ERR;
        }
        pd[pd_entry] |= (uint32_t)tmp;
        /* identity map the new page */
        if (arch_map_page(pd, (uintptr_t)tmp, (uintptr_t)tmp, PAGE_PRESENT | PAGE_RDWR) == GENERIC_ERR) {
            arch_pmm_free(tmp);
            return GENERIC_ERR;
        }
        pmm_used = 1;
    }
    pt = (void *)(pd[pd_entry] & PAGE_MASK);
    /* if page is already mapped, free the table if it was mapped here and return an error */
    if ((pt[pt_entry] & PAGE_MASK) != (uint32_t)NULL) {
        if (pmm_used) {
            arch_pmm_free((void *)(pd[pd_entry] & PAGE_MASK));
            pd[pd_entry] &= ~PAGE_MASK;
        }
        return GENERIC_ERR;
    }
    /* map `phys_addr` to `virt_addr` with `flags` attributes */
    pt[pt_entry] = virt_addr | flags;
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
