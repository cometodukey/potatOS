#ifndef __ACPI_H__
#define __ACPI_H__

#include <kernel/types.h>

typedef struct {
    char signature[8];
    uint8_t checksum;
    char oem[6];
    uint8_t rev;
    uint32_t rsdt_addr;
    /* v2+ only */
    uint32_t length;
    uint64_t xsdt_addr;
    uint8_t ext_checksum;
    uint8_t reserved[3];
} __attribute__((packed)) Rsdp;

void init_acpi(void);

#endif /* __ACPI_H__ */
