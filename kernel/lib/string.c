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

void
strrev(char *str)
{
    int         begin;
    int         end;
    char    tmp;

    begin = 0;
    end = strlen(str) - 1;
    while (begin < end)
    {
        tmp = str[begin];
        str[begin] = str[end];
        str[end] = tmp;
        begin++;
        end--;
    }
}

int
strncmp(const char *s1, const char *s2, size_t n)
{
    size_t  i;

    i = 0;
    while (1)
    {
        if (i >= n || (s1[i] == '\0' && s2[i] == '\0'))
            return (0);
        if (s1[i] != s2[i])
            return ((unsigned char)s1[i] - (unsigned char)s2[i]);
        i++;
    }
}

char *strchr(const char *haystack, int needle)
{
  char *walker;

    walker = (char*)haystack;
    while (*walker || *walker == needle)
    {
        if (*walker == needle)
            return (walker);
        walker++;
    }
    return (0);
}
