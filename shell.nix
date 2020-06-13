let pkgs = import <nixpkgs> { };
in pkgs.mkShell {
  buildInputs = with pkgs; [

    # build tools
    lld
    gnumake
    qemu
    nasm
    grub2
    xorriso
    gdb
    clang_10

    # libraries
    gmp
    mpfr
    libmpc
    isl
    zstd

  ];
}
