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

    /* zero each entry */
    for (i = 0; i < LEN(kernel_page_directory); ++i) {
        kernel_page_directory[i] = !PAGE_PRESENT;
    }

    /* identity map the first MiB */
    for (i = 0; i < 1024; ++i) {
        addr = i * PAGE_SIZE;
        paging_map_page(kernel_page_directory, addr, addr, PAGE_RDWR);
    }
    /* make the zero page not present */
    // paging_map_page(kernel_page_directory, (uintptr_t)NULL, (uintptr_t)NULL, PAGE_RDWR);
    kernel_page_directory[0] |= (PAGE_RDWR | PAGE_PRESENT);

    /* point CR3 to the kernels page directory */
    write_cr3((uint32_t)kernel_page_directory);

    /* enable paging and write protect read-only supervisor pages */
    // write_cr0(read_cr0() | CR0_PG | CR0_WP);

    // 10 bytes into the zero page
    // *(volatile int *)0xa = 0xbadc0de;

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

    // uint32_t *dir = (uint32_t *)0xFFFFF000;
    // uint32_t *table = (uint32_t *)(0xFFC00000 + (dir_index << 12));
    uint32_t *table = (uint32_t *)(dir[dir_index] & PAGE_MASK);

    if (!(dir[dir_index] & PAGE_PRESENT) && create) {
        uint32_t *new_table = arch_pmm_alloc();
        dir[dir_index] = (uint32_t) new_table
            | PAGE_PRESENT | PAGE_RDWR | (flags & PAGE_FLAGS_MASK);
        memset((void*) table, 0, 4096);
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
