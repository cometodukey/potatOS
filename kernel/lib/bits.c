#include <kernel/types.h>

/* set the nth bit in bitmap */
void set_bit(void *bitmap, size_t n) {
    char *array = bitmap;
    array[n / __CHAR_BIT__] |= 1 << (n % __CHAR_BIT__);
}

/* clear the nth bit in bitmap */
void clear_bit(void *bitmap, size_t n) {
    char *array = bitmap;
    array[n / __CHAR_BIT__] &= ~(1 << (n % __CHAR_BIT__));
}

/* test the nth bit in bitmap */
int test_bit(void *bitmap, size_t n) {
    char *array = bitmap;
    return array[n / __CHAR_BIT__] & 1 << (n % __CHAR_BIT__);
}
