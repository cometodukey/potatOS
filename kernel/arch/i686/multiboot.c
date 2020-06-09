#include <kernel/arch/multiboot.h>
#include <kernel/types.h>
#include <kernel/lib/assert.h>
#include <kernel/lib/kprintf.h>
#include <kernel/lib/string.h>
#include <kernel/lib/panic.h>

static void enumerate_mods_list(MultibootModule *mods, uint32_t count);

MultibootModule *initramfs = NULL;
MultibootModule *kernel_syms = NULL;
const char *cmdline = NULL;

void
parse_multiboot_info(uint32_t magic, const MultibootInfo *mb) {
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        PANIC("Wrong Multiboot magic. Expected %p but found %p",
                MULTIBOOT_BOOTLOADER_MAGIC, magic);
    }

    if (mb->flags & MULTIBOOT_INFO_CMDLINE) {
        cmdline = (const char *)mb->cmdline;
    }
    if (mb->flags & MULTIBOOT_INFO_MODS) {
        enumerate_mods_list((MultibootModule *)mb->mods_addr, mb->mods_count);
    } else {
        PANIC("No modules were loaded - no initramfs!");
    }
    if (!(mb->flags & MULTIBOOT_INFO_MEM_MAP)) {
        PANIC("No memory map was passed by the bootloader!");
    }
}

static void
enumerate_mods_list(MultibootModule *mods, uint32_t count) {
    extern MultibootModule *initramfs, *kernel_syms;
    extern const char *version;
    char kernel_map[KPRINT_BUF_SIZE] = {0};
    uint32_t i;

    snprintf(kernel_map, LEN(kernel_map), "spud-%s.map", version);

    for (i = 0; i < count; ++mods, ++i) {
        if (!strcmp((char *)mods->cmdline, "initramfs")) {
            initramfs = mods;
        } else if (!strcmp((char *)mods->cmdline, kernel_map)) {
            kernel_syms = mods;
        }
    }
}
