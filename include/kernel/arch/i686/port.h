#ifndef __PORT_H__
#define __PORT_H__

#include <kernel/types.h>

#define CMOS_PORT 0x70

void out8(uint16_t port, uint8_t data);
void out16(uint16_t port, uint16_t data);
void out32(uint16_t port, uint32_t data);

uint8_t in8(uint16_t port);
uint16_t in16(uint16_t port);
uint32_t in32(uint16_t port);

void io_wait(void);

#endif /* __PORT_H__ */
