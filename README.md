# Yutovo project
Yutovo is a powerful calculator with graphical representation of mathematics operations inside a text editor.

Yutovo logger is a part of the Yutovo project, which aims logging info of other parts of the project.

## Building for Ubuntu

Install the requirements:

```
sudo apt update && sudo apt install -y git cmake g++ pkg-config libspdlog-dev
```
Create the following directories:

```
mkdir -p yutovo/deploy
```
Set this variable to the yutovo directory:

```
export YUTOVO_DEPLOY=~/yutovo/deploy
```

Clone the project in the yutovo dir (select another branch if you want):

```
cd yutovo
git clone -b develop https://github.com/denprog/yutovo-logger.git
```
Create the build directories and build the debug version:

```
mkdir -p build/debug
cd build/debug
cmake -DCMAKE_BUILD_TYPE=Debug ../..
make -sj && make install
```

## Building for Emscripten

Install if you haven't [emscripten](https://emscripten.org/docs/getting_started/downloads.html).

Clone the project in the yutovo dir (select another branch if you want):

```
cd yutovo
git clone -b develop https://github.com/denprog/yutovo-logger.git
```

Create the build directory:

```
cd yutovo-logger
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

Set the VCPKG_ROOT variable to your vcpkg path. Install the requirements:

```
vcpkg install spdlog
```

Clone the project in the yutovo dir (select another branch if you want):

```
cd yutovo
git clone -b develop https://github.com/denprog/yutovo-logger.git
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
