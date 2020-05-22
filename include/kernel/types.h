#ifndef __TYPES_H__
#define __TYPES_H__

#define UNUSED(x) ((void)x)
#define stringify(x) # x
#define xstringify(x) stringify(x)
#define LEN(x) (sizeof(x) / (sizeof(*x)))

#include <stddef.h>
#include <stdint.h>
#include <stdnoreturn.h>

typedef int32_t ssize_t;

_Static_assert(sizeof(void *) == sizeof(uint32_t), "void * != uint32_t");
_Static_assert(__CHAR_BIT__ == 8, "__CHAR_BIT__ != 8");

#endif /* __TYPES_H__ */
