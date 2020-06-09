#ifndef __ISR_H__
#define __ISR_H__

#include <kernel/types.h>

void pic_pit(void);
void pic_keyboard(void);
void pic_cascade(void);
void pic_com2(void);
void pic_com1(void);
void pic_lpt2(void);
void pic_floppy_disk(void);
void pic_lpt1(void);
void pic_rtc(void);
void pic_unused0(void);
void pic_unused1(void);
void pic_unused2(void);
void pic_ps2_mouse(void);
void pic_coprocessor(void);
void pic_primary_ata(void);
void pic_secondary_ata(void);
void pic_irq(unsigned int irq);

#endif /* __ISR_H__ */
