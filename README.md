# serious-graphics-library

For serious programmers.

# dependencies

## X11

Example for debian-based systems:

```shell
sudo apt install \
  libx11-dev \
  libxrandr-dev \
  libxinerama-dev \
  libxcursor-dev \
  libxi-dev
```

## Wayland

Example devShell for NixOS:
```nix
{pkgs, ...}: {
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
```

> [!NOTE] 
> shell.nix is provided

> [!IMPORTANT] 
> generate cmake with `-Dserious-graphics-library_PROTOCOL=WAYLAND`

# credits

- https://learnopengl.com/
- [The Cherno's OpenGL series](https://www.youtube.com/playlist?list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2)
