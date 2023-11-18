with import <nixpkgs> {};
stdenv.mkDerivation {
  name = "my-app";
  src = ./.;
  stdenv = pkgs.clangStdenv;
  buildInputs = [
    llvmPackages_15.clang
    boost.dev
    premake5
    glew
    glfw
    sqlite
    ];
    shellHook = let
    icon = "f121";
    in ''
    export PS1="$(echo -e '\u${icon}') {\[$(tput sgr0)\]\[\033[38;5;228m\]\w\[$(tput sgr0)\]\[\033[38;5;15m\]} \\$ \[$(tput sgr0)\]"
    '';
}
