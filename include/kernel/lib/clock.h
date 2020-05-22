#ifndef __CLOCK_H__
#define __CLOCK_H__

#include <kernel/types.h>

extern uint32_t boot_timestamp;

void sleep(size_t s);

#endif /* __CLOCK_H__ */
