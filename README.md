# music_player

  

# Dowload

To build the project install the qt creator from the link below with the versions mentioned. Both are automatically installed from the link above using the [online installer](https://download.qt.io/official_releases/online_installers/)

> Qt creator - 16.0.1

>

> Qt library - 6.5.0 or newer (6.9.0 recommended)

  

# Build

Tested on the reccomended versions only.

> LLVM-MinGW - 17.0.6 (windows)

>

> Clang - 14 (Linux)

## NixOS
1. Clone the repo
```
git clone https://github.com/KaloyanSvilenov/music_player.git
cd music_player
```
2. Enter dev shell for all dependencies
```
nix develop
```
3. Build the project
```
nix build
```
Extras:
- If you want just the dev shell you can do (without cloning the repo)
```
nix develop github:KaloyanSvilenov/music_player
```
# Useful

Documentation for [media managment](https://www.qt.io/product/qt6/qml-book/ch11-multimedia-multimedia)
