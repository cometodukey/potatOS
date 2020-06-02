#ifndef __MADT_H__
#define __MADT_H__

#include <kernel/types.h>
#include <kernel/acpi/tables/common.h>

/* Multiple APIC Description Table */

typedef struct {
    Sdt header;
    uint32_t local_controller_addr;
    uint32_t flags;
    uint8_t madt_entries_begin;
} __attribute__((packed)) Madt;

typedef struct {
    uint8_t type;
    uint8_t length;
} __attribute__((packed)) MadtEntry;

enum MadtEntryType {
    MADT_LAPIC  = 0,
    MADT_IOAPIC = 1,
    MADT_ISO    = 2,
    MADT_NMI    = 4,
    MADT_LAPIC_ADDRESS_OVERRIDE = 5
};

typedef struct {
    MadtEntry header;
    uint8_t processor_id;
    uint8_t apic_id;
    uint32_t flags;
} __attribute__((packed)) MadtLapic;

typedef struct {
    MadtEntry header;
    uint8_t apic_id;
    uint8_t reserved;
    uint32_t addr;
    uint32_t gsib;
} __attribute__((packed)) MadtIoApic;

typedef struct {
    MadtEntry header;
    uint8_t bus_source;
    uint8_t irq_source;
    uint32_t gsi;
    uint16_t flags;
} __attribute__((packed)) MadtIso;

typedef struct {
    MadtEntry header;
    uint8_t processor;
    uint16_t flags;
    uint8_t lint;
} __attribute__((packed)) MadtNmi;

typedef struct {
    uint16_t reserved;
    uint64_t addr;
} __attribute__((packed)) MadtLapicAddrOverride;

#endif /* __MADT_H__ */
