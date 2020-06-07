# potatOS

An operating system for i686.

# Building from source

## Building the toolchain

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

## Building the kernel

Run `make` to build the kernel and `make iso` to generate an ISO image.
To run the ISO image in QEMU, run `scripts/run`.

Utilities required for building from source are:
* make
* cat
* tr
* cp
* grub-mkrescue
* grub-file
* a POSIX shell
* rm
* mkdir
* wget
* tar
* nasm
