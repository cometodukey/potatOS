#include <kernel/acpi/tables.h>
#include <kernel/lib/panic.h>
#include <kernel/lib/mem.h>

#include <kernel/lib/kprintf.h>
int madt_exists = 0;
Madt *madt = NULL;

void
init_acpi_madt(Madt *ptr) {
    uint8_t *entry;

    madt = ptr;
    if ((uint8_t)memsum(madt, madt->header.length) != 0) {
        PANIC("MADT checksum is invalid!");
    }

    for (entry = (uint8_t *)&madt->madt_entries_begin;
            (uintptr_t)entry < (uintptr_t)madt + madt->header.length; entry += (*entry + 1))
    {
        switch (*entry) {
        case MADT_LAPIC:
            kputs("  Found MADT LAPIC");
            break;
        case MADT_IOAPIC:
            kprintf("  Found MADT IOAPIC");
            break;
        case MADT_ISO:
            kputs("  Found MADT ISO");
            break;
        case MADT_NMI:
            kputs("  Found MADT NMI");
            break;
        case MADT_LAPIC_ADDRESS_OVERRIDE:
            kputs("  Found MADT LAPIC Address Override");
            break;
        default: /* unreachable */
            kprintf("  Unmatched MADT entry (%x)\r\n", *entry);
            //PANIC("%u is an invalid MADT entry!", *entry);
        }
    }
}
