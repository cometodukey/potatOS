TOOLCHAIN_DIR = ../toolchain/i686-tools

STATIC_CFLAGS =  -Wall -Wextra -Wpedantic -Werror
STATIC_CFLAGS += -std=c11 -ffreestanding -nostdlib -nostartfiles
STATIC_CFLAGS += -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-80387 -m32 -march=i686 -fPIE
STATIC_CFLAGS += -fno-stack-protector -fno-omit-frame-pointer -Wl,-z,max-page-size=0x1000
STATIC_CFLAGS += -I../include -DVERSION=$(VERSION)

# -fstack-shuffle
# TODO: have the stack protection link properly

# BUILD_CFLAGS  = $(CFLAGS) $(STATIC_CFLAGS) -g
# BUILD_ASFLAGS = $(ASFLAGS) -f elf32
# BUILD_LDFLAGS = $(LDFLAGS)

BUILD_CFLAGS  = $(STATIC_CFLAGS) -g
BUILD_ASFLAGS = -f elf32
BUILD_LDFLAGS = -L$(TOOLCHAIN_DIR)/lib -l:libgcc.a

CC = $(TOOLCHAIN_DIR)/bin/i686-elf-gcc
LD = $(TOOLCHAIN_DIR)/bin/i686-elf-ld
AS = nasm
