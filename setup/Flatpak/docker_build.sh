apt update
apt install git cmake gcc g++ autoconf automake libtool autotools-dev yacc texinfo pkg-config wget libgtest-dev libgmock-dev zlib1g-dev qtbase5-dev libqt5widgets5 qttools5-dev-tools libfontconfig1-dev

export YUTOVO_DEPLOY=/root/yutovo/deploy/

cd ~
mkdir yutovo && cd yutovo
mkdir deploy
mkdir third-party && cd third-party/

git clone -b v1.15.3 https://github.com/gabime/spdlog.git
cd spdlog/
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/root/yutovo/deploy/ ..
make -sj16 && make install

cd ../../
git clone https://github.com/Tencent/rapidjson.git
cd rapidjson/
mkdir build && cd build
cmake ..
make -sj16 && make install

cd ../../
git clone https://github.com/sethtroisi/libgmp.git
cd libgmp/
git checkout prev_prime
./.bootstrap
./configure --enable-static --disable-shared --enable-cxx --libdir=/root/yutovo/deploy/lib/ --prefix=/root/yutovo/deploy/lib/
make -sj16 && make install

cd ../../
git clone --branch=4.2.1 https://gitlab.inria.fr/mpfr/mpfr.git
cd mpfr/
./autogen.sh
./configure --enable-static --disable-shared --libdir=/root/yutovo/deploy/lib --prefix=/root/yutovo/deploy/lib
make -sj16 && make install

cd ../../
git clone --branch=v1.0.5 https://github.com/denprog/yutovo-logger.git
cd yutovo-logger/
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/root/yutovo/deploy/ -DCMAKE_CXX_FLAGS="-I/root/yutovo/deploy/include" ..
make -sj16 && make install

cd ../../third-party/
wget https://archives.boost.io/release/1.83.0/source/boost_1_83_0.zip
unzip boost_1_83_0.zip
cd boost_1_83_0
./bootstrap.sh --prefix=$PWD/../../deploy --libdir=$PWD/../../deploy/lib --includedir=$PWD/../../deploy/include
./b2 --prefix=$PWD/../../deploy link=static install

cd ..
wget http://downloads.sourceforge.net/mathgl/mathgl-8.0.3.tar.gz
cd mathgl-8.0.3
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug -Denable-openmp=False -Denable-png=False -Denable-opengl=False -DCMAKE_INSTALL_PREFIX=/root/yutovo/deploy/ -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=/root/yutovo/deploy/lib/ -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY=/root/yutovo/deploy/lib/ -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=/root/yutovo/deploy/lib/ ..
make -sj8 && make install

cd ..
git clone --branch=v2.4.4 https://github.com/libharu/libharu.git
cd libharu/
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX=/root/yutovo/deploy/ ..
make -sj16 && make install

cd ..
mkdir stb_image && cd stb_image
wget https://raw.githubusercontent.com/nothings/stb/master/stb_image.h
wget https://raw.githubusercontent.com/nothings/stb/master/stb_image_write.h

cd ../../
git clone -b v1.15.0 https://github.com/google/googletest.git
cd googletest/
cmake -DGTEST_CREATE_SHARED_LIBRARY=1 -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/lib/x86_64-linux-gnu/ ..
make -sj16 && make install

cd ../../
git clone --branch=v1.3.4 https://github.com/denprog/yutovo-calculator.git
cd yutovo-calculator/
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=False -DCMAKE_INSTALL_PREFIX=/root/yutovo/deploy/ ..
make -sj16 && make install

cd ../..
git clone --branch=v1.1.6 https://github.com/denprog/yutovo-solver.git
cd yutovo-solver/
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/root/yutovo/deploy/ ..
make -sj16 && make install

cd ../..
git clone --branch=v1.4.2 https://github.com/denprog/yutovo-editor.git
cd yutovo-editor/
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=False -DCMAKE_INSTALL_PREFIX=/root/yutovo/deploy/ ..
make -sj16 && make install

cd ../..
git clone --branch=v1.4.2 https://github.com/denprog/yutovo-desktop.git
cd yutovo-desktop/
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/root/yutovo/deploy/ ..
make -sj16 && make install

git clone --branch=v1.1.9 https://github.com/denprog/yutovo-server.git
cd yutovo-server/
cp -r library /root/yutovo/deploy/
