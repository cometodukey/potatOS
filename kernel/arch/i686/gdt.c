#include <kernel/arch/gdt.h>
#include <kernel/lib/kprint.h>

// TODO: set userspace gdt

extern void lgdt(uint32_t ptr);
static void gdt_entry(int32_t num, uint32_t base,
                         uint32_t limit, uint8_t access, uint8_t gran);

static GDTPtr gdt_ptr;
static GDTDescriptor gdt_entries[5] = {{0}};

void
init_gdt(void) {
    gdt_ptr.limit = LEN(gdt_entries) * sizeof(GDTDescriptor);
    gdt_ptr.base = (uint32_t)&gdt_entries;

    /* null */
    gdt_entry(GDT_NULL, 0, 0, 0, 0);
    /* kernel code */
    gdt_entry(GDT_KERNEL_CS, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    /* kernel data */
    gdt_entry(GDT_KERNEL_DS, 0, 0xFFFFFFFF, 0x92, 0xCF);
    /* userland - TODO */
    gdt_entry(GDT_USER_CS, 0, 0, 0, 0);
    gdt_entry(GDT_USER_DS, 0, 0, 0, 0);

    lgdt((uint32_t)&gdt_ptr);
    kprintf(" Loaded GDT at %p\r\n", &gdt_ptr);
}

static void
gdt_entry(int32_t num, uint32_t base,
          uint32_t limit, uint8_t access, uint8_t gran)
{
    gdt_entries[num].base_low = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high = (base >> 24) & 0xFF;

    gdt_entries[num].limit_low = (limit & 0xFFFF);
    gdt_entries[num].granularity = (limit >> 16) & 0x0F;

    gdt_entries[num].granularity |= gran & 0xF0;
    gdt_entries[num].access = access;
}
