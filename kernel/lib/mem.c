#include <kernel/types.h>
#include <kernel/lib/mem.h>

void *
memset(void *ptr, int c, size_t len) {
    char *buf = ptr;
    for (; len; --len) {
        *buf++ = (char)c;
    }
    return ptr;
}

void *
memcpy(void *dest, const void *src, size_t n) {
	unsigned char *d = dest;
	const unsigned char *s = src;
    for (; n; n--) *d++ = *s++;
	return dest;
}

void *
memrcpy(void *dest, const void *src, size_t n) {
    size_t counter;
    const char *s = src;
    char *d = dest;

    for (counter = 0; counter < n; ++counter) {
        d[n - counter - 1] = s[n - counter - 1];
    }
    return dest;
}

void *
memmove(void *dest, const void *src, size_t n) {
    return dest > src
        ? memrcpy(dest, src, n)
        : memcpy(dest, src, n);
}

int
memcmp(const void *vl, const void *vr, size_t n) {
	const unsigned char *l=vl, *r=vr;
	for (; n && *l == *r; n--, l++, r++);
	return n ? *l-*r : 0;
}
