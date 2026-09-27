# Yutovo project
Yutovo is a powerful calculator with graphical representation of mathematics operations inside a text editor.

Yutovo solver is designed to distribute calculations coming from the editor.

## Building for Ubuntu

If you haven't yet, build [yutovo-logger](https://github.com/denprog/yutovo-logger) and [yutovo-calculator](https://github.com/denprog/yutovo-calculator).
Install the dependencies:

```
sudo apt update && sudo apt install -y rapidjson-dev
```
This variable should be set to the yutovo directory:

```
export YUTOVO_DEPLOY=~/yutovo/deploy
```

Clone the project in the yutovo dir (select another branch if you want):

```
cd yutovo
git clone -b develop https://github.com/denprog/yutovo-solver.git
```
Create the build directories and build the debug version:

```
mkdir -p build/debug
cd build/debug
cmake -DCMAKE_BUILD_TYPE=Debug ../..
make -sj && make install
```

## Building for Emscripten

If you haven't yet, build [yutovo-logger](https://github.com/denprog/yutovo-logger) and [yutovo-calculator](https://github.com/denprog/yutovo-calculator).

Clone the project in the yutovo dir (select another branch if you want):

```
cd yutovo
git clone -b develop https://github.com/denprog/yutovo-solver.git
```

Create the build directory:

```
cd yutovo-solver
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

If you haven't yet, build [yutovo-logger](https://github.com/denprog/yutovo-logger) and [yutovo-calculator](https://github.com/denprog/yutovo-calculator).

Set the VCPKG_ROOT variable to your vcpkg path. Clone the project in the yutovo dir (select another branch if you want):

```
cd yutovo
git clone -b develop https://github.com/denprog/yutovo-solver.git
```

Create the build directory:

```
cd yutovo-solver
mkdir "build/debug"
cd build/debug
```

Build the project:

```
cmake --build . --config Debug ../..
```
