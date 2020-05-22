#ifndef __MEM_H__
#define __MEM_H__

#include <kernel/types.h>

void *memset(void *buf, int c, size_t len);
void *memcpy(void *dest, const void *src, size_t n);
void *memrcpy(void *dest, const void *src, size_t n);
void *memmove(void *dest, const void *src, size_t n);
int memcmp(const void *vl, const void *vr, size_t n);

#endif /* __MEM_H__ */
