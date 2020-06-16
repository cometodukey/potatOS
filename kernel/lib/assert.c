#include <kernel/types.h>
#include <kernel/lib/kprintf.h>
#include <kernel/arch/arch.h>

noreturn void
__assert_fail(const char *file,
              const char *func,
              size_t line,
              const char *expr)
{
    kprintf("%s:%u: %s: Assertation '%s' failed.\r\n",
            file, line, func, expr);
    for (;;) {
        hang();
    }
}

/* bit of a hack to get around recursive asserts */
noreturn void
__kprint_assert_fail(const char *expr) {
    // FIXME
    UNUSED(expr);
    for (;;) {
        hang();
    }
}
