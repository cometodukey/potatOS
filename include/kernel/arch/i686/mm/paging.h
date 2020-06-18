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

#define PAGE_FLAGS_MASK 0xFFF

void init_paging();
uint32_t *paging_get_page(uint32_t *dir, uintptr_t virt, int flags, int create);
KernelResult paging_map_page(uint32_t *dir, uintptr_t phys, uintptr_t virt, int flags);
// KernelResult paging_map_range(uint32_t *dir, uintptr_t base_phys, uintptr_t base_virt,
//                               size_t len, int flags, uint32_t *failed_map);
void paging_unmap_page(uint32_t *dir, uintptr_t virt, int phys_free);
KernelResult paging_remap_page(uint32_t *dir, uintptr_t phys, uintptr_t virt, int flags);

#endif /* __PAGING_H__ */
