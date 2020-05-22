# potatOS

An operating system for i686.

## Building from source

Run `scripts/mk-toolchain` to build the toolchain.
This will take a little while to complete. Make sure that you have all the required libraries installed, or this will fail.  

Next, run `make` or `make iso`.
To run the ISO image in QEMU, run `scripts/run`.  

Utilities required for building from source are:

* make
* cat
* tr
* cp
* grub-mkrescue
* a POSIX shell
* rm
* mkdir
* wget
* tar
