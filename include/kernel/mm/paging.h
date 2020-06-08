#ifndef __PAGING_H__
#define __PAGING_H__

#include <kernel/types.h>

enum {
    PAGE_NOT_PRESENT = 0 << 0,
    PAGE_PRESENT     = 1 << 0,
    PAGE_RDONLY      = 0 << 1,
    PAGE_RDWR        = 1 << 1,
    PAGE_SUPERVISOR  = 0 << 2,
    PAGE_USER        = 1 << 2,
};

typedef struct {
    uint32_t pages[1024];
} __attribute__((packed)) PageTable;

typedef struct {
    PageTable *tables[1024];
    uintptr_t table_phys[1024];
    uintptr_t addr;
} __attribute__((packed)) PageDirectory;

#endif /* __PAGING_H__ */
