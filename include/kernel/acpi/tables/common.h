#ifndef __COMMON_H__
#define __COMMON_H__

#include <kernel/types.h>

/* System Description Tables */
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
    Sdt root_sdt;
    uint32_t sdt_ptr[];
} __attribute__((packed)) Rsdt;

/* register positions */
typedef struct {
  uint8_t address_space;
  uint8_t bit_width;
  uint8_t bit_offset;
  uint8_t access_size;
  uint64_t address;
} __attribute__((packed)) GenericAddressStructure;

#endif /* __COMMON_H__ */
