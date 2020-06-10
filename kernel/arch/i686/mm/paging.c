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

static uint32_t __align(PAGE_SIZE) page_directory[1024];
static uint32_t __align(PAGE_SIZE) page_tables[1][1024];

void
init_paging(void) {
    size_t i, j;

    kputs("Initialising paging");
    if (!has_pae_support()) {
        PANIC("PAE is not supported!");
    }

    for (i = 0; i < LEN(page_directory); ++i) {
        page_directory[i] = 2;
    }

    for (i = 0; i < LEN(page_tables); ++i) {
        for (j = 0; j < LEN(*page_tables); ++j) {
            page_tables[i][j] = (j * PAGE_SIZE) | 3;
            page_directory[i] = (uint32_t)page_tables[i][j] | 3;
        }
    }

    // TODO: unmap the first MiB

    /* point CR3 to the page directory */
    write_cr3((uint32_t)&page_directory);

    /* enable paging and write protect read-only supervisor pages */
    write_cr0(read_cr0() | CR0_PG | CR0_WP);

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
map_page(uintptr_t phys_addr, uintptr_t virt_addr, int flags) {
    UNUSED(phys_addr);
    UNUSED(virt_addr);
    UNUSED(flags);
    return GENERIC_ERR;
}

KernelResult
unmap_page() {
    return GENERIC_ERR;
}

KernelResult
remap_page() {
    return GENERIC_ERR;
}
