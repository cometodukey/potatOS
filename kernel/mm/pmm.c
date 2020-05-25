#include <kernel/lib/mem.h>
#include <kernel/types.h>
#include <kernel/multiboot.h>
#include <kernel/mm/pmm.h>
#include <kernel/lib/kprint.h>
#include <kernel/lib/panic.h>
#include <kernel/lib/bits.h>
#include <kernel/lib/assert.h>

// TODO: should i support mapping contiguous pages?
// TODO: align any pointers passed to pmm_free with an AND
// TODO: ensure pages under framebuffer and kernel are mapped
// TODO: test 32 bits at a time in pmm_alloc

static void unset_bitmap(size_t bit);
static void set_bitmap(size_t bit);
static int test_bitmap(size_t bit);

/* each bit represents 4KB
   4KB * 256 = 1MiB
   therefore, we need 256 entries per MiB */
static uint32_t initial_pmm_bitmap[256*512];

static void *last_unset = NULL;
static size_t free_pages = 0;
static size_t bitmap_entries = 0;

/* FIXME: this function is a bit hacky */
void
init_pmm(uint32_t mmap_start, size_t mmap_len) {
    MultibootMmapEntry *mmap = (MultibootMmapEntry *)mmap_start;
    uintptr_t base, align;
    size_t usable_mem = 0;

    kputs("Setting up initial memory map");

    /* fill the entire bitmap. later on, free pages over MEM_BASE are unmapped */
    memset(initial_pmm_bitmap, 0xff, LEN(initial_pmm_bitmap));

    for (; (uint32_t)mmap < (mmap_start + mmap_len);
            mmap = (MultibootMmapEntry *)((uint32_t)mmap + mmap->size + sizeof(mmap->size)))
    {
        kprintf(" Size = %p, Address = %p, Length = %p, Type = %u\r\n",
                mmap->size, (size_t)mmap->addr,
                (size_t)mmap->len, mmap->type);

        /* skip any reserved regions */
        if (mmap->type != MULTIBOOT_MEMORY_AVAILABLE) {
            continue;
        }

        /* align base address and length */
        base = (uintptr_t)mmap->addr;
        align = PAGE_SIZE - (base % PAGE_SIZE);
        if (!(base % PAGE_SIZE)) {
            align = 0;
        }
        base += align;
        mmap->len -= align;
        mmap->len -= PAGE_SIZE;
        mmap->len -= mmap->len % PAGE_SIZE;

        /* lazy sanity check */
        assert(!(base % 4096));
        assert(!(mmap->len % 4096));
        assert(mmap->len);

        /* ensure that free pages are aligned >= MIN_MEM */
        if (base < MEM_BASE) {
            if ((base+mmap->len) >= MEM_BASE) {
                while (base < MEM_BASE) {
                    base += PAGE_SIZE;
                    mmap->len -= PAGE_SIZE;
                }
                assert(mmap->len);
                assert(base < (base+mmap->len));
            } else {
                continue;
            }
        }
        assert(base >= MEM_BASE);

        // TODO: check the pages aren't outside the range of the bitmap
        kprintf("  Aligned base = %p, Number of pages = %u\r\n", base, mmap->len / PAGE_SIZE);
        usable_mem += mmap->len;
        for (; mmap->len; base += PAGE_SIZE, mmap->len -= PAGE_SIZE) {
            pmm_free((void *)base);
            ++bitmap_entries;
        }
    }

    if (usable_mem < MIN_MEM) {
        PANIC("Not enough usable RAM! Found %uMiB but at least %uMiB is required",
                BYTE_TO_MIB(usable_mem), BYTE_TO_MIB(MIN_MEM));
    }
    kprintf(" Found %uMiB of usable RAM (%u pages)\r\n", BYTE_TO_MIB(usable_mem), usable_mem / PAGE_SIZE);
}

void *
pmm_alloc(void) {
    size_t i;
    if (!test_bitmap((size_t)last_unset / PAGE_SIZE)) {
        i = (size_t)last_unset / PAGE_SIZE;
        goto free_page;
    }
    for (i = BITMAP_BASE; i < bitmap_entries; ++i) {
        if (!test_bitmap(i)) {
            goto free_page;
        }
    }
    PANIC("Out of physical memory!");
free_page:
    set_bitmap(i);
    return ((void *)(uintptr_t)(PAGE_SIZE * i));
}

void *
pmm_zalloc(void) {
    void *page = pmm_alloc();
    memset(page, 0, PAGE_SIZE);
    return page;
}

void
pmm_free(void *page) {
    size_t bit = (size_t)page / PAGE_SIZE;
    assert((uintptr_t)page >= MEM_BASE);
    /* check if the page is mapped */
    if (!test_bitmap(bit)) {
        PANIC("Attempt to unmap an unmapped page!");
    }
    unset_bitmap(bit);
    last_unset = page;
}

static void
set_bitmap(size_t bit) {
    set_bit(initial_pmm_bitmap, bit);
    --free_pages;
}

static void
unset_bitmap(size_t bit) {
    clear_bit(initial_pmm_bitmap, bit);
    ++free_pages;
}

static int
test_bitmap(size_t bit) {
    return test_bit(initial_pmm_bitmap, bit);
}
