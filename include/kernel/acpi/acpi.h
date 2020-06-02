#ifndef __ACPI_H__
#define __ACPI_H__

#include <kernel/types.h>
#include <kernel/acpi/tables.h>

void init_acpi(void);
void init_acpi_fadt(Fadt *fadt);

#endif /* __ACPI_H__ */
