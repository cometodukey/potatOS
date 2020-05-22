#ifndef __STRING_H__
#define __STRING_H__

#include <kernel/types.h>

size_t strlen(const char *s);
int strcmp(const char *dst, const char *src);
int strncmp(const char *s1, const char *s2, size_t n);
char *strchr(const char *haystack, int needle);
void strrev(char *str);

#endif /* __STRING_H__ */
