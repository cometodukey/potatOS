#include <kernel/types.h>
#include <kernel/i686/cpu.h>
#include <kernel/mm/paging.h>
#include <kernel/lib/errno.h>
#include <kernel/lib/assert.h>
#include <kernel/mm/pmm.h>

uintptr_t __attribute__((aligned(PAGE_SIZE))) pdpte_start[1024];

void
init_paging(void) {
    /* point CR3 to the start of the PDPTE */
    write_cr3((uint32_t)pdpte_start);

    /* set the paging flags in CR0 and CR4 */
    write_cr0(read_cr0() | CR0_PG);
    write_cr4(read_cr4() | CR4_PAE);
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
