#include <kernel/acpi/tables.h>
#include <kernel/lib/panic.h>
#include <kernel/lib/mem.h>

#include <kernel/lib/kprintf.h>
int madt_exists = 0;
Madt *madt = NULL;

void
init_acpi_madt(Madt *ptr) {
    uint8_t *entry;
    MadtLapic *lapic;
    MadtIoApic *ioapic;
    MadtIso *iso;

    madt = ptr;
    if ((uint8_t)memsum(madt, madt->header.length) != 0) {
        PANIC("MADT checksum is invalid!");
    }

    for (entry = (uint8_t *)&madt->madt_entries_begin;
            (uintptr_t)entry < (uintptr_t)madt + madt->header.length; entry += (*entry + 1))
    {
        switch (*entry) {
        case MADT_LAPIC:
            lapic = (MadtLapic *)entry;
            kputs("  Found MADT LAPIC");
            kprintf("   Processor ID: %x\r\n", lapic->processor_id);
            kprintf("   APIC ID:      %x\r\n", lapic->processor_id);
            kprintf("   Flags:        %x\r\n", lapic->flags);
            break;
        case MADT_IOAPIC:
            ioapic = (MadtIoApic *)entry;
            kprintf("  Found MADT IOAPIC");
            kprintf("   APIC ID: %x\r\n", ioapic->apic_id);
            kprintf("   Address: %p\r\n", ioapic->addr);
            kprintf("   GSIB:    %x\r\n", ioapic->gsib);
            break;
        case MADT_ISO:
            iso = (MadtIso *)entry;
            kputs("  Found MADT ISO");
            kprintf("   Bus Source: %x\r\n", iso->bus_source);
            kprintf("   IRQ Source: %x\r\n", iso->irq_source);
            kprintf("   GSI:        %x\r\n", iso->gsi);
            kprintf("   Flags:      %x\r\n", iso->flags);
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
