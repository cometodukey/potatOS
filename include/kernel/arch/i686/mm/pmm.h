#ifndef __PMM_H__
#define __PMM_H__

#define PAGE_SIZE 4096
#define PAGE_MASK (0xfffff000)
#define MEM_BASE (1 << 20)
#define BITMAP_BASE (MEM_BASE / PAGE_SIZE)
#define MIN_MEM (MEM_BASE * 256)
#define BYTE_TO_MIB(num) (((num) / 1024) / 1024)

void init_pmm(uint32_t mmap_start, size_t mmap_len);
void *arch_pmm_alloc(void);
void *arch_pmm_zalloc(void);
void arch_pmm_free(void *page);

#endif /* __PMM_H__ */
