#include <kernel/acpi/acpi.h>
#include <kernel/types.h>
#include <kernel/lib/mem.h>
#include <kernel/lib/panic.h>
#include <kernel/lib/kprintf.h>

Rsdp *rsdp = NULL;

void
init_acpi(void) {
    uintptr_t walker;
    uint32_t checksum = 0;

    kputs("Initialising ACPI");
    // TODO: check EBDA if not found here
    for (walker = 0xE0000; walker != 0xFFFFF; walker += 16) {
        if (!memcmp((void *)walker, "RSD PTR ", 8)) {
            kprintf(" Found RSDP at %p\r\n", walker);
            rsdp = (Rsdp *)walker;
            break;
        }
    }

    if (rsdp == NULL) {
        PANIC("ACPI not supported!");
    }

    checksum = memsum(rsdp, 20);
    if ((uint8_t)checksum != 0) {
        PANIC("ACPI checksum is invalid!");
    }

    kputs(" RSDP info:");
    kprintf("  Signature:    %.8s\r\n", rsdp->signature);
    kprintf("  Checksum:     %x\r\n", rsdp->checksum);
    kprintf("  OEM:          %.6s\r\n", rsdp->oem);
    kprintf("  Revision:     %x\r\n", rsdp->rev);
    kprintf("  RSDT address: %p\r\n", rsdp->rsdt_addr);
    if (rsdp->rev >= 2) {
        /* v2+ */
        // TODO: validate ext_checksum
        kprintf(" Length:            %u\r\n", rsdp->length);
        kprintf(" XSDT address:      %p\r\n", rsdp->xsdt_addr);
        kprintf(" Extended checksum: %x\r\n", rsdp->ext_checksum);
    }
}
