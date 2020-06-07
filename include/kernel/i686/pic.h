#ifndef __PIC_H__
#define __PIC_H__

#include <kernel/types.h>

/* PIC ports */
enum {
    MASTER_PIC_COM  = 0x20,
    MASTER_PIC_DATA = 0x21,
    SLAVE_PIC_COM   = 0xA0,
    SLAVE_PIC_DATA  = 0xA1
};

/* PIC commands
   ICW = Initialsation Command Word */
enum {
    ICW1_ICW4       = 0x1,  /* ICW4 */
    ICW1_SINGLE     = 0x2,  /* single (cascade) mode */
    ICW1_INTERVAL4  = 0x4,  /* call address interval 4 */
    ICW1_LEVEL      = 0x08, /* level triggered (edge) mode */
    ICW1_INIT       = 0x10, /* initialisation */
    ICW4_8086       = 0x01, /* 8086/88 (MCS-80/85) mode */
    ICW4_AUTO       = 0x02, /* auto (normal) EOI */
    ICW4_BUF_SLAVE  = 0x08, /* buffered mode/slave */
    ICW4_BUF_MASTER = 0x0C, /* buffered mode/master */
    ICW4_SFNM       = 0x10, /* special fully nested */
    PIC_EOI         = 0x20  /* end of interrupt - tells the PIC you have received the interrupt */
};

void init_pic(uint8_t, uint8_t);
void pic_eoi(unsigned char irq);
void pic_disable(void);

#endif /* __PIC_H__ */
