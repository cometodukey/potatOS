#include <kernel/acpi/acpi.h>
#include <kernel/types.h>
#include <kernel/lib/mem.h>
#include <kernel/lib/panic.h>
#include <kernel/lib/kprintf.h>
#include <kernel/lib/errno.h>

static KernelResult find_rsdp_signature(uintptr_t start, uintptr_t end, void **found);
static void init_acpi_tables(void);

Rsdp *rsdp = NULL;
Rsdt *rsdt;

void
init_acpi(void) {
    uint16_t ebda;
    size_t checksum;

    kputs("Initialising ACPI");
    if (find_rsdp_signature(0xE0000, 0xFFFFF, (void **)&rsdp) == GENERIC_ERR) {
        ebda = *(volatile uint16_t *)0x40e;
        if (find_rsdp_signature(ebda, ebda + 4096, (void **)&rsdp) == GENERIC_ERR) {
            PANIC("ACPI not supported");
        }
    }
    kprintf(" Found RSDP at %p\r\n", rsdp);

    checksum = memsum(rsdp, 20);
    if ((uint8_t)checksum != 0) {
        PANIC("ACPI checksum is invalid!");
    }

    // TODO: support ACPI v2+
    kputs(" RSDP info:");
    kprintf("  Signature:    %.8s\r\n", rsdp->signature);
    kprintf("  Checksum:     %x\r\n",   rsdp->checksum);
    kprintf("  OEM ID:       %.6s\r\n", rsdp->oem_id);
    kprintf("  Revision:     %u\r\n",   rsdp->rev);
    kprintf("  RSDT address: %p\r\n",   rsdp->rsdt_addr);
    if (rsdp->rev >= 2) {
        /* v2+ */
        // TODO: validate ext_checksum
        kprintf(" Length:            %u\r\n", rsdp->length);
        kprintf(" XSDT address:      %p\r\n", rsdp->xsdt_addr);
        kprintf(" Extended checksum: %x\r\n", rsdp->ext_checksum);
    }

    rsdt = (Rsdt *)rsdp->rsdt_addr;

    init_acpi_tables();
}

static KernelResult
find_rsdp_signature(uintptr_t start, uintptr_t end, void **found) {
    uintptr_t walker;
    for (walker = start; walker != end; walker += 16) {
        if (!memcmp((void *)walker, "RSD PTR ", 8)) {
            *found = (void *)walker;
            return GENERIC_SUCCESS;
        }
    }
    return GENERIC_ERR;
}

static void
init_acpi_tables(void) {
    uint8_t checksum = memsum((void *)&rsdt->sdt, rsdt->sdt.length);
    if (checksum != 0) {
        PANIC("Invalid SDT checksum!");
    }

    kputs(" SDT info:");
    kprintf("  Signature:        %.4s\r\n", rsdt->sdt.signature);
    kprintf("  Length:           %u\r\n",   rsdt->sdt.length);
    kprintf("  Revision:         %u\r\n",   rsdt->sdt.rev);
    kprintf("  Checksum:         %x\r\n",   rsdt->sdt.checksum);
    kprintf("  OEM ID:           %.6s\r\n", rsdt->sdt.oem_id);
    kprintf("  OEM Table ID:     %.8s\r\n", rsdt->sdt.oem_table_id);
    kprintf("  OEM Revision:     %u\r\n",   rsdt->sdt.oem_rev);
    kprintf("  Creator ID:       %u\r\n",   rsdt->sdt.creator_id);
    kprintf("  Creator Revision: %u\r\n",   rsdt->sdt.creator_rev);
}
