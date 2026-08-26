# Yutovo project
Yutovo is a powerful calculator with graphical representation of mathematics operations inside a text editor.

Yutovo desktop is an application for Linux and Windows that implements computing and text editing. It is based on Qt and written in C++, as well as other libraries in the project.

The base features are:

* Graphical representation of mathematical formulas
* Math WYSIWYG editor
* Numbers and calculation results can have (almost) any number of digits and accuracy
* Following types of numbers are supported: real, integer, fractional, complex
* Measure of an angle can be changed
* Commonly used mathematical functions and constants are supported
* User variables and functions are supported
* Units, unit systems and translating values between them are supported
* Automatic recalculation of expressions
* Symbolic calculations
* Text editor with basic features
* Include documents
* Graphs of functions
* Syntax highlighting
* Library of examples and visual documentation
* Online version with the same features

## Screenshots
<img src="https://yutovo.com/screenshots/2025-05-07_20-27.png" width="500">

<img src="https://yutovo.com/screenshots/2025-05-07_20-31.png" width="500">

<img src="https://yutovo.com/screenshots/2025-07-12_07-17.png" width="500">

<img src="https://yutovo.com/screenshots/2025-05-07_20-29_2.png" width="500">

<img src="https://yutovo.com/screenshots/2025-05-07_20-31_1.png" width="500">

<img src="https://yutovo.com/screenshots/2025-07-12_07-17_1.png" width="500">

<img src="https://yutovo.com/screenshots/2025-07-12_07-19.png" width="500">

<img src="https://yutovo.com/screenshots/2025-12-23_17-20_1.png" width="500">

<img src="https://yutovo.com/screenshots/2025-12-23_17-20.png" width="500">

<img src="https://yutovo.com/screenshots/2026-05-31_21-21.png" width="500">

## Installing from Flathub
[![Get it on Flathub](https://flathub.org/api/badge?locale=en)](https://flathub.org/en/apps/com.yutovo.yutovo)

## Installing from Snap Store
[![Get it from the Snap Store](https://snapcraft.io/en/dark/install.svg)](https://snapcraft.io/yutovo)

## Building for Ubuntu

If you haven't yet, build [yutovo-logger](https://github.com/denprog/yutovo-logger), [yutovo-calculator](https://github.com/denprog/yutovo-calculator) and [yutovo-solver](https://github.com/denprog/yutovo-solver), [yutovo-editor](https://github.com/denprog/yutovo-editor).
Install the dependencies:

```
sudo update && sudo apt install -y qttools5-dev-tools
```

This variable should be set to the yutovo directory:

```
export YUTOVO_DEPLOY=~/yutovo/deploy
```
Clone the project in the yutovo dir (select another branch if you want):

```
cd yutovo
git clone -b develop https://github.com/denprog/yutovo-desktop.git
```
Create the build directories and build the debug version:

```
mkdir -p build/debug
cd build/debug
cmake -DCMAKE_BUILD_TYPE=Debug ../..
make -sj && make install
```

## Building for Fedora

### Install build dependencies

```bash
sudo dnf install -y rpmdevtools cmake gcc-c++ make pkgconf-pkg-config qt5-qtbase-devel qt5-qttools-devel qt5-qtbase-private-devel \
    qt5-qtdeclarative-devel qt5-qtsvg-devel qt5-qtx11extras-devel gmp-devel mpfr-devel libmpc-devel boost-devel spdlog-devel \
    rapidjson-devel desktop-file-utils libappstream-glib hicolor-icon-theme stb_image-devel stb_image_write-devel fontconfig-devel
```
Build and install giac: https://www-fourier.univ-grenoble-alpes.fr//~parisse/install_en.html.

### Download sources

```bash
cd src/setup
spectool -g -R yutovo-desktop.spec
```

### Building

```bash
rpmbuild -ba yutovo-desktop.spec
```

### Installing

```bash
sudo rpm -Uvh ~/rpmbuild/RPMS/x86_64/yutovo-*.rpm
```

### Notes

- Fedora 44 uses GCC 16 with C23 as default. The spec handles compatibility via `-std=gnu17` for bundled libraries (GMP, MPFR, MPC).
- Bundled libraries built from source: `mathgl-8.0.3`, `libharu-2.4.4`, `gmp-6.3.0`, `mpfr-4.2.1`, `mpc-1.3.1`.
- For OBS/Open Build Service automation see `src/setup/` directory.

## Building for Arch

Install the base dependiences.
```
sudo pacman -S --needed base-devel git
```

Clone the repo:
```
git clone https://aur.archlinux.org/yutovo.git
cd yutovo
```

Make and install the package:
```
makepkg -si
```

## Building for Windows

If you haven't yet, build [yutovo-logger](https://github.com/denprog/yutovo-logger), [yutovo-calculator](https://github.com/denprog/yutovo-calculator) and [yutovo-solver](https://github.com/denprog/yutovo-solver), [yutovo-editor](https://github.com/denprog/yutovo-editor).

Set the VCPKG_ROOT variable to your vcpkg path. Install the requirements:
```
vcpkg install qt5-base qt5-widgets qt5-gui
```

Clone the project in the yutovo dir (select another branch if you want):

```
cd yutovo
git clone -b develop https://github.com/denprog/yutovo-desktop.git
```

Create the build directory:

```
cd yutovo-desktop
mkdir "build/debug"
cd build/debug
```

Build the project:

```
cmake --build . --config Debug ../..
```
