# Yutovo project
Yutovo is a powerful calculator with graphical representation of mathematics operations inside a text editor.

Yutovo desktop is an application for Linux and Windows that implements computing.

[![Get it on Flathub](https://flathub.org/assets/badges/flathub-badge-en.png)](https://flathub.org/en/apps/com.yutovo.yutovo)

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

## Building for Windows

If you haven't yet, build [yutovo-logger](https://github.com/denprog/yutovo-logger), [yutovo-calculator](https://github.com/denprog/yutovo-calculator) and [yutovo-solver](https://github.com/denprog/yutovo-solver), [yutovo-editor](https://github.com/denprog/yutovo-editor).

Install the requirements:
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
mkdir -p build/debug
cd build/debug
```

Build the project:

```
cmake -DCMAKE_BUILD_TYPE=Debug ../..
make -sj && make install
```
