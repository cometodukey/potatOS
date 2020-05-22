#include <kernel/types.h>
#include <kernel/lib/string.h>

size_t
strlen(const char *s1) {
    const char *s2;
    for (s2 = s1; *s2 != '\0'; ++s2);
    return s2 - s1;
}

int
strcmp(const char *dst, const char *src) {
	for (; *dst == *src && *dst; dst++, src++);
	return *(unsigned char *)dst - *(unsigned char *)src;
}
