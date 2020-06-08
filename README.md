# potatOS

An operating system for i686.

# Building from source

## Building the kernel

You can build the kernel with GCC or Clang.
If you want to build with Clang, make sure that your environments `CC` is set to `clang`.
If you want to build with GCC, it is recommended to build GCC and libgcc using `scripts/mk-toolchain`.  

The Makefile also relies on the `i686-elf-ld` existing, so you should at least build this before building the kernel.
It is built in the same way as GCC, by running `scripts/mk-toolchain`.  

If you have problems building the toolchain, read [link](#1-building-the-toolchain) section.  

Run `make` to build the kernel and `make iso` to generate an ISO image.
To run the ISO image in QEMU, run `scripts/run`.

The following utilities are required for building from source:

* GNU make
* POSIX shell
* POSIX coreutils
* nasm
* wget
* tar
* grub-mkrescue
* grub-file


## Building the toolchain

To build the toolchain, run `scripts/mk-toolchain`.
This will download the sources for GDB, binutils and GCC, then configure and build them.

GCC requires several libraries to build:

* gmp
* mpfr
* mpc
* isl
* zstd

Ensure you have all of these installed, otherwise the script will fail.

GCC might also fail to build if any of these libraries are in non-standard locations.
To solve this, append the library paths to the LD_LIBRARY_PATH environment variable when building and running GCC.  
`LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/path/to/lib" scripts/mk-toolchain`
