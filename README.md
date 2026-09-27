# Yutovo project
Yutovo is a powerful calculator with graphical representation of mathematics operations inside a text editor.

Yutovo editor combines the capabilities of editing text, mathematical formulas and performing calculations.

This is a completely custom editor that provides functions for displaying and editing text and formulas. It has an interface for modifying, customizing, and displaying the internal structure, as well as for executing code of formulas and obtaining calculation results.

## Building for Ubuntu

If you haven't yet, build [yutovo-logger](https://github.com/denprog/yutovo-logger), [yutovo-calculator](https://github.com/denprog/yutovo-calculator) and [yutovo-solver](https://github.com/denprog/yutovo-solver).

Install the dependencies:

```
sudo apt update && sudo apt install -y qtbase5-dev qtchooser qt5-qmake libgtest-dev libgmock-dev libqt5widgets5 libmpfr-dev
```
This variable should be set to the yutovo directory:

```
export YUTOVO_DEPLOY=~/yutovo/deploy
```
Clone the project in the yutovo dir (select another branch if you want):

```
cd yutovo
git clone -b develop https://github.com/denprog/yutovo-editor.git
```
Create the build directories and build the debug version:

```
mkdir -p build/debug
cd build/debug
cmake -DCMAKE_BUILD_TYPE=Debug ../..
make -sj && make install
```
Run the tests:

```
./test/yutovo-editor_tests
```

## Building for Emscripten

If you haven't yet, build [yutovo-logger](https://github.com/denprog/yutovo-logger), [yutovo-calculator](https://github.com/denprog/yutovo-calculator) and [yutovo-solver](https://github.com/denprog/yutovo-solver).

Clone the project in the yutovo dir (select another branch if you want):

```
cd yutovo
git clone -b develop https://github.com/denprog/yutovo-editor.git
```

Create the build directory:

```
cd yutovo-editor
mkdir -p build_web/debug
cd build_web/debug
```
Set these variables:

```
export YUTOVO_DEPLOY=~/yutovo/deploy
source ~/emsdk/emsdk_env.sh
```

Build the project:

```
emcmake cmake -DCMAKE_BUILD_TYPE=Debug ../..
make -sj && make install
```

## Building for Windows

If you haven't yet, build [yutovo-logger](https://github.com/denprog/yutovo-logger), [yutovo-calculator](https://github.com/denprog/yutovo-calculator) and [yutovo-solver](https://github.com/denprog/yutovo-solver).

Set the YUTOVO_DEPLOY variable to your yutovo/deploy path.
In the yutovo/third-party download stb_image/stb_image.h and stb_image/stb_image_write.h source files.
Set the VCPKG_ROOT variable to your vcpkg path. Clone the project in the yutovo dir (select another branch if you want):

```
cd yutovo
git clone -b develop https://github.com/denprog/yutovo-editor.git
```

Create the build directory:

```
cd yutovo-editor
mkdir "build/debug"
cd build/debug
```

Build the project:

```
cmake --build . --config Debug ../..
```
