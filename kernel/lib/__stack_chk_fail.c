#include <kernel/types.h>
#include <kernel/lib/panic.h>

// TODO: fix this

uintptr_t __stack_chk_guard = 0xdeadbeef;

noreturn void
__stack_chk_fail(void) {
    PANIC("Kernel stack smashing detected", 0);
}
