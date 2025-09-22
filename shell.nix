{pkgs ? import <nixpkgs> {}}:
pkgs.mkShell {
  buildInputs = with pkgs; [
    extra-cmake-modules

    wayland
    wayland-scanner
    wayland-protocols
    libxkbcommon
    libffi

    libglvnd
  ];
}
