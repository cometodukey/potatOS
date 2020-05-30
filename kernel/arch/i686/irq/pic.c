#include <kernel/arch/pic.h>
#include <kernel/arch/port.h>
#include <kernel/lib/kprint.h>
#include <kernel/types.h>

void
remap_pic(uint8_t pic0_offset, uint8_t pic1_offset) {
    char pic0_mask, pic1_mask;

    kprintf(" Remapping PIC IRQs with offsets %u and %u\r\n", pic0_offset, pic1_offset);

    /* save masks */
    pic0_mask = in8(MASTER_PIC_DATA);
    pic1_mask = in8(SLAVE_PIC_DATA);

    /* start the init sequence */
    out8(MASTER_PIC_COM, ICW1_INIT | ICW1_ICW4);
    io_wait();
    out8(SLAVE_PIC_COM, ICW1_INIT | ICW1_ICW4);
    io_wait();

    /* set the vector offsets */
    out8(MASTER_PIC_DATA, pic0_offset);
    io_wait();
    out8(SLAVE_PIC_DATA, pic1_offset);
    io_wait();

    /* tell the master that the slave is on IRQ2 */
    out8(MASTER_PIC_DATA, 4);
    io_wait();
    /* tell the slave its cascade identity */
    out8(SLAVE_PIC_DATA, 2);
    io_wait();

    out8(MASTER_PIC_DATA, ICW4_8086);
    io_wait();
    out8(SLAVE_PIC_DATA, ICW4_8086);
    io_wait();

    /* restore masks */
    out8(MASTER_PIC_DATA, pic0_mask);
    out8(SLAVE_PIC_DATA, pic1_mask);
}

void
pic_eoi(unsigned char irq) {
    if (irq >= 8) {
        out8(SLAVE_PIC_COM,PIC_EOI);
    }
    out8(MASTER_PIC_COM,PIC_EOI);
}

void
pic_disable(void) {
    out8(SLAVE_PIC_DATA, 0xff);
    out8(MASTER_PIC_DATA, 0xff);
}
