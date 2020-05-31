#include <kernel/events/isr.h>
#include <kernel/lib/assert.h>
#include <kernel/lib/kprint.h>

const char *pic_names[] = {
    "PIT",
    "Keyboard",
    "Cascade", /* should never be raised */
    "COM2",
    "COM1",
    "LPT2",
    "Floppy Disk"
    "LPT1",
    "RTC",
    "Unused 0",
    "Unused 1",
    "Unused 2",
    "PS2 Mouse",
    "Co-processor",
    "Primary ATA",
    "Secondary ATA", 
};

void
pic_irq(unsigned int irq) {
    assert(irq < LEN(pic_names));
    kprintf("Received legacy PIC IRQ: %s (%u). Ignoring\r\n", pic_names[irq], irq);
}
