# Configuring the environment

You can build the kernel with an LLVM or GNU based toolchain.

## LLVM

To use the LLVM toolchain, set `CC=clang` and `LD=lld` in your environment before running make.  

## GNU

To use the GNU toolchain, make sure that your environments `CC` and `LD` are ***not*** set to `clang` and `lld`, respectively.
You will need to run `scripts/mk-toolchain` to build the toolchain.
If you have problems building the toolchain, read [this](toolchain.md) article.  

The following additional utilities are required for building and running the kernel:

* GNU make
* POSIX shell
* POSIX coreutils
* nasm
* wget
* tar
* grub-mkrescue
* QEMU

# Building the kernel

`make default` - Build the kernel
`make iso`     - Build the kernel and generate an ISO
`make info`    - Display build flags
`make clean`   - Clean build files

# Running the ISO image

`scripts/run`       - Run the ISO image in QEMU
`scripts/run debug` - Run the ISO image in QEMU with a GDB hook
