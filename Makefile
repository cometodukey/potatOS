# Remove Makes builtin rules and variables
.SUFFIXES:
MAKEFLAGS += --no-builtin-rules --no-builtin-variables

TOOLCHAIN = toolchain/i686-tools

KERNEL_VERSION = 0.1.0
KERNEL_ELF     = spud-$(KERNEL_VERSION).elf
KERNEL_SYMS    = spud-$(KERNEL_VERSION).map

UNAME = $(shell uname)

# LD, LDFLAGS
LDFLAGS = -L$(TOOLCHAIN)/lib
ifeq ($(LD), lld)
ifeq ($(UNAME), Linux)
LD = ld.lld
else ifeq ($(UNAME), Darwin)
LD = ld64.lld
else
# FIXME
LD = unknown
endif
else
LD = $(TOOLCHAIN)/bin/i686-elf-ld
endif

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
CFLAGS += -fsanitize=undefined

ifeq ($(CC), clang)
CFLAGS += --target=i686-pc-unknown-elf -x c
else
CC = $(TOOLCHAIN)/bin/i686-elf-gcc
LDFLAGS += -l:libgcc.a
endif

# AS, ASFLAGS
AS      = nasm
ASFLAGS = -f elf32

# Enumerate kernel sources
KERNEL_CSRCS = $(shell find kernel/ -type f -name "*.c") lib/ubsan.c
KERNEL_ASRCS = $(shell find kernel/ -type f -name "*.S")
KERNEL_OBJS  = $(KERNEL_CSRCS:.c=.o) $(KERNEL_ASRCS:.S=.o)

# Default target - only build the kernel
.PHONY: default
default: $(KERNEL_ELF)

.PHONY: info
info:
	$(info CC = $(CC))
	$(info AS = $(AS))
	$(info LD = $(LD))
	$(info CFLAGS  = $(CFLAGS))
	$(info ASFLAGS = $(ASFLAGS))
	$(info LDFLAGS = $(LDFLAGS))

# Build the kernel
$(KERNEL_ELF): $(KERNEL_OBJS)
	$(info LD $@)
	@$(LD) $(LDFLAGS) -T kernel/kernel.ld -o kernel/$@ $(KERNEL_OBJS)
	$(info GENSYMS $@)
	@scripts/gen-symlist kernel/$(KERNEL_ELF) kernel/$(KERNEL_SYMS)

# Generate a bootable ISO
# TODO - clean up this rule
.PHONY: iso
iso: $(KERNEL_ELF)
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

# Build the sources

.SUFFIXES: .c .o
.c.o:
	$(info CC $@)
	@$(CC) $(CFLAGS) -c -o $@ $<

.SUFFIXES: .S .o
.S.o:
	$(info AS $@)
	@$(AS) $(ASFLAGS) -o $@ $<
