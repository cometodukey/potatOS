#include <kernel/types.h>
#include <kernel/lib/panic.h>

noreturn void __stack_chk_fail_local(void);
noreturn void __stack_chk_fail(void);

uintptr_t __stack_chk_guard = 0xdeadbeef;

noreturn void
__stack_chk_fail_local(void) {
    __stack_chk_fail();
}

noreturn void
__stack_chk_fail(void) {
    PANIC("Kernel stack smashing detected");
}
