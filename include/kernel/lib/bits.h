#ifndef __BITS_H__
#define __BITS_H__

#include <kernel/types.h>

/* these functions expect the bitmaps to be at least n bits long */

/* set the nth bit in bitmap */
void set_bit(void *bitmap, size_t n);
/* clear the nth bit in bitmap */
void clear_bit(void *bitmap, size_t n);
/* test the nth bit in bitmap */
int test_bit(void *bitmap, size_t n);

#endif /* __BITS_H__ */
