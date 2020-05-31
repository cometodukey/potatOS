#include <kernel/types.h>
#include <kernel/lib/kprintf.h>
#include <kernel/arch/idle.h>
#include <kernel/console/vt100.h>

noreturn void
__assert_fail(const char *file,
              const char *func,
              size_t line,
              const char *expr)
{
    kprintf("%s:%u: %s: Assertation '%s' failed.\r\n",
            file, line, func, expr);
    hang();
}

/* bit of a hack to get around recursive asserts */
noreturn void
__kprint_assert_fail(const char *expr) {
    vt100_puts("kprint Assertation '");
    vt100_puts(expr);
    vt100_puts("' failed!\r\n");

    hang();
}
