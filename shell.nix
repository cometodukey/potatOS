let pkgs = import <nixpkgs> { };
in pkgs.mkShell {
  buildInputs = with pkgs; [

    # build tools
    gnumake
    qemu
    nasm
    grub2
    xorriso
    gdb

    # libraries
    gmp
    mpfr
    libmpc
    isl
    zstd

  ];
}
