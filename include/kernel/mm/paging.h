#ifndef __PAGING_H__
#define __PAGING_H__

#include <kernel/types.h>
#include <kernel/attributes.h>

#define CPUID_PAE_BIT (1 << 6)

enum {
    PAGE_PRESENT     = 1 << 0,
    PAGE_RDWR        = 1 << 1,
    PAGE_USER        = 1 << 2,
};

typedef struct {
    uint32_t pages[1024];
} __packed PageTable;

typedef struct {
    PageTable *tables[1024];
    uintptr_t table_phys[1024];
    uintptr_t addr;
} __packed PageDirectory;

void init_paging();

#endif /* __PAGING_H__ */
