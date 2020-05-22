#ifndef __KMALLOC_H__
#define __KMALLOC_H__

#include <kernel/types.h>

// TODO: checksum? or hash?
typedef struct HeapChunk {
    struct HeapChunk *next;
    size_t size;
    int free;
    /* padding bytes before the chunk header */
    char pad;
} HeapChunk;

void *kmalloc(size_t size);
void *kzmalloc(size_t size);
void *krealloc(void *ptr, size_t *new_size);
void kfree(void *ptr);

#endif /* __KMALLOC_H__ */
