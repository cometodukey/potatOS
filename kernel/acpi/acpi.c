#include <kernel/acpi/acpi.h>
#include <kernel/acpi/tables.h>
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
    uint8_t checksum;

    kputs("Initialising ACPI");
    if (find_rsdp_signature(0xE0000, 0xFFFFF, (void **)&rsdp) == GENERIC_ERR) {
        kputs(" Did not find RSDP in 0xE0000 to 0xFFFFF");
        ebda = *(volatile uint16_t *)0x40e;
        if (find_rsdp_signature(ebda, ebda + 4096, (void **)&rsdp) == GENERIC_ERR) {
            kputs(" Did not find RSDP in first 4KB of EBDA");
            PANIC("ACPI not supported");
        }
    }
    kprintf(" Found Root System Description Pointer at %p\r\n", rsdp);

    checksum = memsum(rsdp, 20);
    if (checksum != 0) {
        PANIC("ACPI checksum is invalid!");
    }

    kprintf("  OEM ID:   %.6s\r\n", rsdp->oem_id);
    kprintf("  Revision: %u\r\n", rsdp->rev);

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
    size_t i = 0, entries;
    Sdt *header;
    uint8_t checksum = memsum((void *)&rsdt->root_sdt, rsdt->root_sdt.length);
    if (checksum != 0) {
        PANIC("Invalid SDT checksum!");
    }
    entries = (rsdt->root_sdt.length - sizeof(Sdt)) / 4;

    kprintf(" Found Root System Description Table at %p\r\n", rsdt->root_sdt);
    for (i = 0; i < entries; ++i) {
        header = (Sdt *)(rsdt->sdt_ptr[i]);
        if (!memcmp(header, "FACP", 4)) {
            kprintf(" Found Fixed ACPI Description Table at %p\r\n", header);
            init_acpi_fadt((Fadt *)header);
        } else {
            kprintf(" Found SDT %.4s at %p\r\n", header, header);
        }
    }
}
