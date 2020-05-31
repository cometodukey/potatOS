.POSIX:
.PHONY: all info clean run

include config.mk

ifndef VERSION
$(error VERSION is not set)
endif

ifndef KERNEL_IMG
$(error KERNEL_IMG is not set)
endif

CSRCS =  $(shell find . -type f -name "*.c")
ASRCS =  $(shell find . -type f -name "*.S")
OBJS  =  $(CSRCS:.c=.o) $(ASRCS:.S=.o)
#OBJS  += $(ASRCS:.S=.o)

spud: $(OBJS)
	$(LD) $(BUILD_LDFLAGS) -T linker.ld -o $(KERNEL_IMG) $(OBJS)

%.o: %.S
	$(AS) $(BUILD_ASFLAGS) -o $@ $<

%.o: %.c
	$(CC) $(BUILD_CFLAGS) -c -o $@ $<

clean:
	-rm $(OBJS) $(KERNEL_IMG)
