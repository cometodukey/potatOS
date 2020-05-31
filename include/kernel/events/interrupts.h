#ifndef __INTERRUPTS_H__
#define __INTERRUPTS_H__

#include <kernel/types.h>

typedef struct {
   uint16_t base_low;
   uint16_t selector;
   uint8_t  pad; /* unused */
   uint8_t  type;
   uint16_t base_high;
} __attribute__((packed)) IDTDescriptor;

typedef struct {
   uint16_t limit;
   uint32_t base;
} __attribute__((packed)) IDTPtr;

void init_idt(void);
void sti(void);
void cli(void);
void enable_nmi(void);
void disable_nmi(void);

#endif /* __INTERRUPTS_H__ */
