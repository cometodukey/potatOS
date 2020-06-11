#ifndef __PAGING_H__
#define __PAGING_H__

#include <kernel/types.h>
#include <kernel/attributes.h>
#include <kernel/lib/errno.h>

#define CPUID_PAE_BIT (1 << 6)

/* 32 bit paging */
enum {
    PAGE_PRESENT = 1 << 0,
    PAGE_RDWR    = 1 << 1,
    PAGE_USER    = 1 << 2,
    PAGE_WT      = 1 << 3,
    PAGE_CD      = 1 << 4
};

void init_paging();
KernelResult arch_map_page(uint32_t page_directory[1024], uintptr_t phys_addr, uintptr_t virt_addr, int flags);

#endif /* __PAGING_H__ */
