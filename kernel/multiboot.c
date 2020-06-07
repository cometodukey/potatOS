#include <kernel/multiboot.h>
#include <kernel/types.h>
#include <kernel/lib/assert.h>
#include <kernel/lib/kprintf.h>
#include <kernel/lib/string.h>
#include <kernel/lib/panic.h>

static void iterate_mods_list(MultibootModule *mods, uint32_t count);

void
parse_multiboot_info(uint32_t magic, const MultibootInfo *mb) {
    const char *cmdline = (char *)mb->cmdline;

    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        PANIC("Wrong Multiboot magic. Expected %p but found %p",
                MULTIBOOT_BOOTLOADER_MAGIC, magic);
    }

    kprintf("Multiboot info:\r\n");
    kprintf(" Header flags:        %p\r\n", mb->flags);
    if (mb->flags & MULTIBOOT_INFO_BOOT_LOADER_NAME) {
        kprintf(" Bootloader name:     %s\r\n", mb->boot_loader_name);
    }
    if (mb->flags & MULTIBOOT_INFO_CMDLINE) {
        kprintf(" Command line:        %s\r\n", cmdline);
    }
    if (mb->flags & MULTIBOOT_INFO_MODS) {
        kprintf(" Module count:        %p\r\n", mb->mods_count);
        kprintf(" Modules address:     %p\r\n", mb->mods_addr);
        iterate_mods_list((MultibootModule *)mb->mods_addr, mb->mods_count);
    } else {
        PANIC("No modules were loaded - no initramfs!");
    }
    if (mb->flags & MULTIBOOT_INFO_MEM_MAP) {
        kprintf(" Memory map length:   %p\r\n", mb->mmap_length);
        kprintf(" Memory map address:  %p\r\n", mb->mmap_addr);
    } else {
        PANIC("No memory map was passed by the bootloader!");
    }
    if (mb->flags & MULTIBOOT_INFO_FRAMEBUFFER_INFO) {
        // TODO: kprint colour info
        /* the bootloader should set this below 4GB, but check to be safe */
        assert((mb->framebuffer_addr & UINT32_MAX) == mb->framebuffer_addr);
        kprintf(" Framebuffer address: %p\r\n", mb->framebuffer_addr);
        kprintf(" Framebuffer pitch:   %u\r\n", mb->framebuffer_pitch);
        kprintf(" Framebuffer width:   %u\r\n", mb->framebuffer_width);
        kprintf(" Framebuffer height:  %u\r\n", mb->framebuffer_height);
        kprintf(" Framebuffer BPP:     %u\r\n", mb->framebuffer_bpp);
        kprintf(" Framebuffer type:    %u\r\n", mb->framebuffer_type);
    }
}

static void
iterate_mods_list(MultibootModule *mods, uint32_t count) {
    extern MultibootModule *initramfs, *kernel_syms;
    extern const char *version;
    char kernel_map[KPRINT_BUF_SIZE] = {0};
    uint32_t i;

    snprintf(kernel_map, LEN(kernel_map), "spud-%s.map", version);

    for (i = 0; i < count; ++mods, ++i) {
        kprintf("  Module %u:\r\n", i);
        kprintf("   Command line: %s\r\n", mods->cmdline);
        kprintf("   Size:         %p\r\n",
                mods->mod_end - mods->mod_start);
        if (!strcmp((char *)mods->cmdline, "initramfs")) {
            initramfs = mods;
        } else if (!strcmp((char *)mods->cmdline, kernel_map)) {
            kernel_syms = mods;
        }
    }
}
