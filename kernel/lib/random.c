#include <kernel/types.h>
#include <kernel/lib/assert.h>

// TODO: write an actually good number generator

static uint32_t xorshift32(uint32_t state);

void
init_random(void) {
	// TODO: fix the screaming issue here
    extern uintptr_t __stack_chk_guard;
    __stack_chk_guard = xorshift32(__stack_chk_guard);
}

static uint32_t
xorshift32(uint32_t state) {
    assert(state);
	state ^= state << 13;
	state ^= state >> 17;
	state ^= state << 5;
	return state;
}
