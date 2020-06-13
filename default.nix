with import <nixpkgs> { };

stdenv.mkDerivation {
  name = "potatOS";
  src = ./.;

  buildInputs = with pkgs; [
    # libraries
    gmp
    mpfr
    libmpc
    isl
    zstd

    # build phase
    clang_10
    lld
    nasm

    # iso building
    grub2
    xorriso
  ];

  buildPhase = ''
    make CC=clang LD=ld.lld iso
  '';

  installPhase = ''
    mkdir -p $out/iso/
    mv potatOS.iso $out/iso/
  '';
}
