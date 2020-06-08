#include <kernel/types.h>
#include <kernel/i686/cpu.h>
#include <kernel/mm/paging.h>

static PageDirectory *pdpte[1024];
static PageDirectory pde[1024];
static PageTable pt[1024];

_Static_assert(LEN(pde) >= LEN(pdpte), "PDE is smaller than PDPTE!");

void
init_paging(void) {
    size_t i;
    UNUSED(pt);
    for (i = 0; i < LEN(pdpte); ++i) {
        pdpte[i] = &pde[i];
    }
    write_cr3((uint32_t)&pdpte);
    write_cr0(read_cr0());
}
