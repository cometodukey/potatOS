#ifndef __ACPI_H__
#define __ACPI_H__

#include <kernel/types.h>

typedef struct {
    char signature[8];
    uint8_t checksum;
    char oem_id[6];
    uint8_t rev;
    uint32_t rsdt_addr;
    /* v2+ only */
    uint32_t length;
    uint64_t xsdt_addr;
    uint8_t ext_checksum;
    uint8_t reserved[3];
} __attribute__((packed)) Rsdp;

typedef struct {
    char signature[4];
    uint32_t length;
    uint8_t rev;
    uint8_t checksum;
    char oem_id[6];
    char oem_table_id[8];
    uint32_t oem_rev;
    uint32_t creator_id;
    uint32_t creator_rev;
} __attribute__((packed)) Sdt;

typedef struct {
    Sdt sdt;
    uint32_t *sdt_ptr;
} __attribute__((packed)) Rsdt;

void init_acpi(void);

#endif /* __ACPI_H__ */
