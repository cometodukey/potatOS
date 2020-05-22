#ifndef __ASSERT_H__
#define __ASSERT_H__

#include <kernel/types.h>

#define assert(expr) \
    ((expr) ? (void)0 : __assert_fail(__FILE__, __func__, __LINE__, #expr))

#define kprint_assert(expr) \
    ((expr) ? ((void)0) : __kprint_assert_fail(#expr))

#define ASSERT_NOT_REACHED \
    __assert_fail(__FILE__, __func__, __LINE__, "NOT_REACHED")

noreturn void __assert_fail(const char *file,
                            const char *func,
                            size_t line,
                            const char *expr);
noreturn void __kprint_assert_fail(const char *expr);

#endif /* __ASSERT_H__ */
