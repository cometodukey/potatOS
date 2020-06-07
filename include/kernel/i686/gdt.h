#ifndef __GDT_H__
#define __GDT_H__

#include <kernel/types.h>

#define GDT_NULL 0
#define GDT_KERNEL_CS 1
#define GDT_KERNEL_DS 2
#define GDT_USER_CS 3
#define GDT_USER_DS 4

typedef struct {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access;
    uint8_t  granularity;
    uint8_t  base_high;
} __attribute__((packed)) GDTDescriptor;

typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) GDTPtr;

void init_gdt(void);

#endif /* __GDT_H__ */
