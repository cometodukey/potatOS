.POSIX:

# Clear Make builtins
MAKEFLAGS += --no-builtin-rules --no-builtin-variables
.SUFFIXES:

TOOLCHAIN := $(shell pwd)/toolchain/i686-tools

KERNEL_VERSION := 0.1.0
KERNEL_ELF     := spud-$(KERNEL_VERSION).elf
KERNEL_SYMS    := spud-$(KERNEL_VERSION).map

# CFLAGS, CC
CFLAGS  = -std=c11 -Wall -Wextra -Wpedantic -Werror
CFLAGS += -Wno-unused-command-line-argument
CFLAGS += -g -O0 -march=i686 -m32 -fPIE
CFLAGS += -ffreestanding -nostdlib -nostartfiles
CFLAGS += -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-80387
CFLAGS += -fstack-protector-all -fno-omit-frame-pointer
CFLAGS += -fno-builtin -fno-common
CFLAGS += -Wl,-z,max-page-size=0x1000
CFLAGS += -Iinclude/
CFLAGS += -DKERNEL_VERSION="$(KERNEL_VERSION)"

ifeq ($(CC), clang)
CFLAGS += --target=i686-pc-unknown-elf -x c
else
CC := $(TOOLCHAIN)/bin/i686-elf-gcc
endif # $(CC), clang

# AS, ASFLAGS
AS      := nasm
ASFLAGS := -f elf32

# LD, LDFLAGS
LD      := $(TOOLCHAIN)/bin/i686-elf-ld
LDFLAGS := -L$(TOOLCHAIN)/lib -l:libgcc.a

# Enumerate kernel sources
KERNEL_CSRCS := $(shell find kernel/ -type f -name "*.c")
KERNEL_ASRCS := $(shell find kernel/ -type f -name "*.S")
KERNEL_OBJS  := $(KERNEL_CSRCS:.c=.o) $(KERNEL_ASRCS:.S=.o)

.PHONY: default
default: $(KERNEL_ELF)

.PHONY: all
all: $(KERNEL_ELF) iso

# Build the kernel
.PHONY: $(KERNEL_ELF)
$(KERNEL_ELF): $(KERNEL_OBJS)
	$(info LD $@)
	@$(LD) $(LDFLAGS) -T kernel/linker.ld -o kernel/$@ $(KERNEL_OBJS)
	$(info GENSYMS $@)
	@scripts/gen-symlist kernel/$(KERNEL_ELF) kernel/$(KERNEL_SYMS)

# Generate a bootable ISO
# TODO - clean up this rule
.PHONY: iso
iso: $(KERNEL_ELF)
	grub-file --is-x86-multiboot kernel/$(KERNEL_ELF)
	cp kernel/$(KERNEL_SYMS) fsroot/boot
	cp kernel/$(KERNEL_ELF)  fsroot/boot
	scripts/gen-grubcfg $(KERNEL_ELF) $(KERNEL_VERSION) fsroot/boot/grub/grub.cfg
	grub-mkrescue -o potatOS.iso fsroot/

# Clean build files
.PHONY: clean
clean:
	@rm -f fsroot/boot/$(KERNEL_SYMS) 	\
		fsroot/boot/$(KERNEL_ELF)		\
		kernel/$(KERNEL_SYMS)			\
		kernel/$(KERNEL_ELF)			\
		$(KERNEL_OBJS)					\
		potatOS.iso

%.o: %.c
	$(info CC $@)
	@$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.S
	$(info AS $@)
	@$(AS) $(ASFLAGS) -o $@ $<
