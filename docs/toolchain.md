# Building the toolchain

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
