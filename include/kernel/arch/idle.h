#ifndef __IDLE_H__
#define __IDLE_H__

#include <kernel/arch/idle.h>

void idle(void);
noreturn void hang(void);

#endif /* __IDLE_H__ */
