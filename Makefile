.POSIX:
.PHONY: all info kernel iso clean

SCRIPTS = scripts

VERSION = $(shell cat version.txt | tr -d '[:space:]')
KERNEL_IMG = spud-$(VERSION).elf
export VERSION
export KERNEL_IMG

all: kernel iso

kernel:
	$(MAKE) $(MAKEFLAGS) -C kernel/
	grub-file --is-x86-multiboot kernel/$(KERNEL_IMG)
	$(SCRIPTS)/gen-symlist kernel/$(KERNEL_IMG) fsroot/boot/spud-$(VERSION).map
	cp -f kernel/$(KERNEL_IMG) fsroot/boot

iso: kernel
	$(SCRIPTS)/gen-grubcfg $(KERNEL_IMG) $(VERSION) $(VERSION)
	grub-mkrescue -o potatOS.iso fsroot/

clean:
	+$(MAKE) -C kernel/ clean
	-rm fsroot/boot/spud-$(VERSION).map potatOS.iso fsroot/boot/$(KERNEL_IMG)
