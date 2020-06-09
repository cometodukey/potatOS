#ifndef __KMALLOC_H__
#define __KMALLOC_H__

#include <kernel/types.h>

void init_kmalloc(void);
void *kmalloc(size_t size);
void *kzmalloc(size_t size);
void *krealloc(void *ptr, size_t *new_size);
void kfree(void *ptr);

#endif /* __KMALLOC_H__ */
