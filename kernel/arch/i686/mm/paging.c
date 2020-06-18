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

static int has_pae_support(void);
static void invlpg(uintptr_t addr);

static uint32_t __align(PAGE_SIZE) kernel_page_directory[1024];

void
init_paging(void) {
    size_t i;
    uint32_t failed_addr = 0;
    UNUSED(failed_addr);

    if (!has_pae_support()) {
        PANIC("PAE is not supported!");
    }

    /* zero each entry */
    for (i = 0; i < LEN(kernel_page_directory); ++i) {
        kernel_page_directory[i] = 0;
    }

    /* identity map the first 32MiB (except the zero page!) */
    // if (paging_map_range(kernel_page_directory, 0x1000, 0x1000,
    //                      (MEM_BASE * 32) / PAGE_SIZE, PAGE_RDWR, &failed_addr) == GENERIC_ERR)
    // {
    //     PANIC("Failed to identity map %.8p", failed_addr);
    // }
    for (i = 0; i < 1024; ++i) {
        uintptr_t addr = i * PAGE_SIZE;
        if (paging_map_page(kernel_page_directory, addr, addr, PAGE_RDWR) == GENERIC_ERR) {
            kprintf("Failed to map %.8p to %.8p\r\n", addr, addr);
        }
    }
    /* unmap the zero page - no NULL pointers */
    paging_unmap_page(kernel_page_directory, (uintptr_t)NULL, 0);

    kernel_page_directory[0] |= (PAGE_RDWR | PAGE_PRESENT);

    /* point CR3 to the kernels page directory */
    write_cr3((uint32_t)kernel_page_directory);

    /* enable paging and write protect read-only supervisor pages */
    write_cr0(read_cr0() | CR0_PG | CR0_WP);

    void *new = arch_pmm_zalloc();
    if (paging_map_page(kernel_page_directory, (uintptr_t)new, UINT32_MAX/2, PAGE_RDWR) == GENERIC_ERR) {
        kputs("failed to map page");
    }
    // *(volatile int *)(UINT32_MAX/2) = 0xbadc0de;
    // if (paging_remap_page(kernel_page_directory, (uintptr_t)new, UINT32_MAX/2, 0) == GENERIC_ERR) {
    //     kputs("failed to remap page");
    // }
    // *(volatile int *)(UINT32_MAX/2) = 0;

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

uint32_t *
paging_get_page(uint32_t *dir, uintptr_t virt, int flags, int create) {
    if (virt % PAGE_SIZE) {
        PANIC("Attempt to get page table entry from unaligned address %.8p!", virt);
    }

    // TODO - macros to get the indexes?
    uint32_t dir_index = virt >> 22;
    uint32_t table_index = (virt >> 12) & 0x3FF;
    uint32_t *table = (uint32_t *)(dir[dir_index] & PAGE_MASK);

    if (!(dir[dir_index] & PAGE_PRESENT) && create) {
        uint32_t *new_table = arch_pmm_zalloc();
        if (new_table == NULL) {
            return NULL;
        }
        table = new_table;
        dir[dir_index] = (uint32_t)table | PAGE_PRESENT | PAGE_RDWR | (flags & PAGE_FLAGS_MASK);
    }

    if (dir[dir_index] & PAGE_PRESENT) {
        return &table[table_index];
    }

    return NULL;
}

KernelResult
paging_map_page(uint32_t *dir, uintptr_t phys, uintptr_t virt, int flags) {
    uint32_t *page = paging_get_page(dir, virt, flags, 1);
    if (page == NULL) {
        kprintf("paging_get_page failed to get %.8p\r\n", virt);
        return GENERIC_ERR;
    }

    /* enforce 4096 byte alignment */
    if (phys % PAGE_SIZE) {
        PANIC("Attempt to map unaligned physical address %.8p!", phys);
    }
    if ((uintptr_t)dir % PAGE_SIZE) {
        PANIC("Attempt to map into unaligned page directory %.8p!", dir);
    }

    /* detect previous mappings */
    if ((*page & PAGE_PRESENT) || ((*page & PAGE_MASK) != 0)) {
        PANIC("Attempt to map already mapped page %.8p -> %.8p\r\nPrevious mapping %.8p -> %.8p",
              virt, phys, virt, *page & PAGE_MASK);
    }

    /* map the page */
    *page = phys | PAGE_PRESENT | (flags & PAGE_FLAGS_MASK);

    /* invalidate the new mapping if `dir` is the current page directory */
    if (read_cr3() == (uint32_t)dir) {
        invlpg(virt);
    }

    return GENERIC_SUCCESS;
}

// FIXME - this causes some weird bugs
// KernelResult
// paging_map_range(uint32_t *dir, uintptr_t base_phys, uintptr_t base_virt,
//                  size_t len, int flags, uint32_t *failed_map) {
//     for (; len; --len, base_phys += PAGE_SIZE, base_virt += PAGE_SIZE) {
//         if (paging_map_page(dir, base_phys, base_virt, flags) == GENERIC_ERR) {
//             if (failed_map != NULL) {
//                 *failed_map = base_virt;
//             }
//             return GENERIC_ERR;
//         }
//     }
//     return GENERIC_SUCCESS;
// }

void
paging_unmap_page(uint32_t *dir, uintptr_t virt, int free_phys) {
    uint32_t *page = paging_get_page(dir, virt, 0, 0);
    if (page != NULL) {
        if (!(*page & PAGE_PRESENT)) {
            PANIC("Attempt to unmap %.8p which is already unmapped!", virt);
        }
        if (free_phys) {
            arch_pmm_free((void *)(*page & PAGE_MASK));
        }
        *page = 0;
    }
}

KernelResult
paging_remap_page(uint32_t *dir, uintptr_t phys, uintptr_t virt, int flags) {
    uint32_t *page = paging_get_page(dir, virt, flags, 0);
    if (page == NULL || (*page & PAGE_MASK) == (uintptr_t)NULL) {
        return GENERIC_ERR;
    }
    *page = phys | (flags & PAGE_FLAGS_MASK);
    if (read_cr3() == (uintptr_t)dir) {
        invlpg(virt);
    }
    return GENERIC_SUCCESS;
}
