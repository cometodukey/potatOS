#include <kernel/types.h>

uint8_t
in8(uint16_t port) {
    uint8_t data;
    __asm__ volatile("inb %1, %0" : "=a"(data) : "d"(port));
    return data;
}

uint16_t
in16(uint16_t port) {
    uint16_t data;
    __asm__ volatile("inw %1, %0" : "=a"(data) : "d"(port));
    return data;
}

uint32_t
in32(uint16_t port) {
    uint32_t data;
    __asm__ volatile("inl %1, %0" : "=a"(data) : "d"(port));
    return data;
}

void
out8(uint16_t port, uint8_t data) {
    __asm__ volatile("outb %0, %1" :: "a"(data), "d"(port));
}

void
out16(uint16_t port, uint16_t data) {
    __asm__ volatile("outw %0, %1" :: "a"(data), "d"(port));
}

void
out32(uint16_t port, uint32_t data) {
    __asm__ volatile("outl %0, %1" :: "a"(data), "d"(port));
}
