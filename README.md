# Yutovo project
Yutovo is a powerful calculator with graphical representation of mathematics operations inside a text editor.

Yutovo calculator is a library for calculations using its own simple syntax.

## Features
   * Expression has a standard mathematical syntax based on nested parentheses.
   * All arithmetic and logical operations are supported.
   * Numbers can have (almost) unlimited length.
   * Numbers can have (almost) unlimited precision.
   * User-defined variables, functions, and units are supported.
   * The following number types are supported: real, fractional, integer, complex, and arrays of real numbers.
   * All the basic arithmetic, trigometric, and hyperbolic functions are built-in.
   * Basic physical units of the following systems are built-in: SI, Russian, and American.
   * Conversion between units is supported.
   * The following angular units are supported: degrees, radians, and grads.
   * Basic physical constants are built-in.
   * Automatic unit selection is supported.
   * Units are presented in the following languages: Russian, English, Spanish.

## Building for Ubuntu

If you haven't yet, build [yutovo-logger](https://github.com/denprog/yutovo-logger).
Install the dependencies:

```
sudo apt update && sudo apt install -y libboost-iostreams1.83-dev libgtest-dev libgmock-dev libmpfr-dev
```
This variable should be set to the yutovo directory:

```
export YUTOVO_DEPLOY=~/yutovo/deploy
```

Clone the project in the yutovo dir (select another branch if you want):

```
cd yutovo
git clone -b develop https://github.com/denprog/yutovo-calculator.git
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
./test/yutovo-calculator_tests
```

## Building for Emscripten

Install boost:
```
wget https://archives.boost.io/release/1.81.0/source/boost_1_81_0.zip
unzip boost_1_81_0.zip

cd boost_1_81_0
./bootstrap.sh --prefix=$PWD/../../deploy --libdir=$PWD/../../deploy/lib --includedir=$PWD/../../deploy/include
./b2 --prefix=$PWD/../../deploy link=static install
./b2 -q toolset=emscripten link=static variant=release threading=single
```

Install gmp:
```
git clone https://github.com/sethtroisi/libgmp.git
cd libgmp
git checkout prev_prime
./.bootstrap
emconfigure ./configure --enable-static --disable-shared --host wasm32 --enable-cxx --libdir=/home/denis/programs/Math/yutovo/deploy/wasm/ --prefix=/home/denis/programs/Math/yutovo/deploy/
make -sj4 && make install
```

Install mpfr:
```
git clone --branch=4.2.1 https://gitlab.inria.fr/mpfr/mpfr.git
cd mpfr
export CPPFLAGS='-pthread -s USE_PTHREADS=1 -s PTHREAD_POOL_SIZE=2'
emconfigure ./configure --enable-static --host wasm32 --disable-shared --libdir=/home/denis/programs/Math/yutovo/deploy/wasm/ --prefix=/home/denis/programs/Math/yutovo/deploy/ --with-gmp-build=/home/denis/programs/Math/yutovo/third_party/libgmp/
make -sj4 && make install
```

Clone the project in the yutovo dir (select another branch if you want):
```
cd yutovo
git clone -b develop https://github.com/denprog/yutovo-calculator.git
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

If you haven't yet, build [yutovo-logger](https://github.com/denprog/yutovo-logger).
Set the VCPKG_ROOT variable to your vcpkg path. Install the requirements:

```
vcpkg install boost gtest gmock mpfr
```
Clone the project in the yutovo dir (select another branch if you want):

```
cd yutovo
git clone -b develop https://github.com/denprog/yutovo-calculator.git
```

Create the build directory:

```
cd yutovo-calculator
mkdir "build/debug"
cd build/debug
```

Build the project:

```
cmake --build . --config Debug ../..
```
