Name: yutovo
Version: 1.7.1
Release: alt1
Summary: Multifunctional visual calculator with graphical representation of mathematical formulas
Group: Sciences/Mathematics

License: GPL-3.0-only
URL: https://yutovo.com
Source0: https://github.com/denprog/yutovo-desktop/archive/refs/tags/v%{version}.tar.gz#/%{name}-desktop-%{version}.tar.gz

# Yutovo submodules
Source1: https://github.com/denprog/yutovo-logger/archive/refs/tags/v1.0.6.tar.gz#/yutovo-logger-1.0.6.tar.gz
Source2: https://github.com/denprog/yutovo-calculator/archive/refs/tags/v1.6.1.tar.gz#/yutovo-calculator-1.6.1.tar.gz
Source3: https://github.com/denprog/yutovo-solver/archive/refs/tags/v1.3.1.tar.gz#/yutovo-solver-1.3.1.tar.gz
Source4: https://github.com/denprog/yutovo-editor/archive/refs/tags/v%{version}.tar.gz#/yutovo-editor-%{version}.tar.gz
Source5: https://github.com/denprog/yutovo-library/archive/refs/tags/v1.1.1.tar.gz#/yutovo-library-1.1.1.tar.gz
Source6: https://github.com/libharu/libharu/archive/refs/tags/v2.4.4.tar.gz#/libharu-2.4.4.tar.gz
# giac 2.0.0, sha256 3e7fa1c281a21ab74ed66ef247faffe5a105bc11be1f7715f31b4368ce8dcbc8
Source7: https://www-fourier.univ-grenoble-alpes.fr/~parisse/giac/giac_stable.tgz#/giac-2.0.0.tar.gz
# gmp source is used only for gmpxx.h: ALT ships it in libgmpxx-devel, the header is version-independent C++ wrappers over the system libgmp 6.3.0
Source12: https://ftp.gnu.org/gnu/gmp/gmp-6.3.0.tar.xz
# boost 1.83 headers: yutovo-solver uses the boost::process v1 API whose boost/process.hpp pulls boost/asio headers; system boost-devel (1.85) on ALT
# does not ship the asio module at all
Source9: https://archives.boost.io/release/1.83.0/source/boost_1_83_0.zip
Source17: http://downloads.sourceforge.net/mathgl/mathgl-8.0.3.tar.gz

BuildRequires: cmake >= 3.16
BuildRequires: gcc-c++
BuildRequires: make
BuildRequires: pkg-config
BuildRequires: qt5-base-devel
BuildRequires: qt5-tools-devel
BuildRequires: qt5-declarative-devel
BuildRequires: qt5-svg-devel
BuildRequires: qt5-x11extras-devel
BuildRequires: boost-devel
BuildRequires: libspdlog-devel
BuildRequires: rapidjson-devel
BuildRequires: desktop-file-utils
BuildRequires: libappstream-devel
BuildRequires: hicolor-icon-theme
BuildRequires: libstb-devel
BuildRequires: fontconfig-devel
BuildRequires: libgmp-devel
BuildRequires: libmpfr-devel
BuildRequires: zlib-devel
BuildRequires: libpng-devel

Requires: qt5-qtbase-gui
Requires: hicolor-icon-theme

%description
Yutovo is a multifunctional calculator with graphical representation of mathematics
operations inside a text editor. Based on Qt and written in C++.

Features:
- Graphical representation of mathematical formulas
- Math WYSIWYG editor
- Arbitrary precision numbers
- Symbolic calculations (giac)
- Units and unit conversion
- Graphs of functions

%prep
# The spec does its own extraction (no %setup), so rpmbuild never cleans the
# build tree; remove leftovers of previous builds (e.g. 1.6.2 deploy libs)
rm -rf %{_builddir}/%{name}-deploy %{_builddir}/submodules \
    %{_builddir}/third_party %{_builddir}/yutovo-desktop-*
mkdir -p %{_builddir}/%{name}-deploy
export YUTOVO_DEPLOY=%{_builddir}/%{name}-deploy

# Extract sources
tar -xzf %{SOURCE0} -C %{_builddir}
mkdir -p %{_builddir}/submodules
tar -xzf %{SOURCE1} -C %{_builddir}/submodules
tar -xzf %{SOURCE2} -C %{_builddir}/submodules
tar -xzf %{SOURCE3} -C %{_builddir}/submodules
tar -xzf %{SOURCE4} -C %{_builddir}/submodules
tar -xzf %{SOURCE5} -C %{_builddir}/submodules
mkdir -p %{_builddir}/third_party
tar -xzf %{SOURCE6} -C %{_builddir}/third_party
tar -xzf %{SOURCE17} -C %{_builddir}/third_party
tar -xzf %{SOURCE7} -C %{_builddir}/third_party
unzip -q %{SOURCE9} -d %{_builddir}/third_party
# The giac build script and the config.h patch ship in the desktop tarball
cd %{_builddir}/third_party/giac-2.0.0
cp %{_builddir}/%{name}-desktop-%{version}/setup/Flatpak/build_giac.sh .
cp %{_builddir}/%{name}-desktop-%{version}/setup/Flatpak/giac_config_h.diff .
# ALT's gcc enables -Werror=return-type by default; giac 2.0.0 has a few
# control-reaches-end-of-non-void paths (graphe.cc) that upstream builds tolerate
sed -i 's|CFLAGS = -O2 -DNDEBUG|CFLAGS = -Wno-error=return-type -O2 -DNDEBUG|' build_giac.sh
patch -p1 < giac_config_h.diff

%build
export YUTOVO_DEPLOY=%{_builddir}/%{name}-deploy
# giac and the yutovo-* libraries live in the deploy dir; giac_imported and
# libstdc++fs are resolved there via linker scripts installed by build_giac.sh
export LIBRARY_PATH=${YUTOVO_DEPLOY}/lib
# gmpxx.h (installed into the deploy prefix below) must be visible to giac and
# to every component compiling against the giac headers
export CPATH=${YUTOVO_DEPLOY}/include${CPATH:+:${CPATH}}

# Build yutovo-logger
mkdir -p %{_builddir}/submodules/yutovo-logger-1.0.6/build
cd %{_builddir}/submodules/yutovo-logger-1.0.6/build
cmake .. -DCMAKE_INSTALL_PREFIX=${YUTOVO_DEPLOY} -DCMAKE_BUILD_TYPE=Release
make %{?_smp_mflags}
make install

# ALT ships gmpxx.h in libgmpxx-devel only; take the plain header from the gmp tarball (same 6.3.0 as the system library) into the deploy prefix
tar -xJf %{SOURCE12} -C %{_builddir}/third_party gmp-6.3.0/gmpxx.h
install -m644 %{_builddir}/third_party/gmp-6.3.0/gmpxx.h ${YUTOVO_DEPLOY}/include/

# Build giac (static, Release); the script honours FLATPAK_DEST as the install prefix and FLATPAK_BUILDER_MAX_JOBS for the parallel build
cd %{_builddir}/third_party/giac-2.0.0
FLATPAK_DEST=${YUTOVO_DEPLOY} FLATPAK_BUILDER_MAX_JOBS=4 bash build_giac.sh
# yutovo-calculator requires both release and debug library names at configure time
cp -f ${YUTOVO_DEPLOY}/lib/libgiac.a ${YUTOVO_DEPLOY}/lib/libgiacd.a

# Build yutovo-calculator
mkdir -p %{_builddir}/submodules/yutovo-calculator-1.6.1/build
cd %{_builddir}/submodules/yutovo-calculator-1.6.1/build
cmake .. -DCMAKE_INSTALL_PREFIX=${YUTOVO_DEPLOY} -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DCMAKE_C_COMPILER=gcc
make %{?_smp_mflags}
make install

# Build yutovo-solver
# boost 1.83 headers go first via CPATH: the solver needs the boost::process
# v1 API (boost/process.hpp + boost/asio), absent from system boost-devel
mkdir -p %{_builddir}/submodules/yutovo-solver-1.3.1/build
cd %{_builddir}/submodules/yutovo-solver-1.3.1/build
CPATH="%{_builddir}/third_party/boost_1_83_0${CPATH:+:${CPATH}}" \
    cmake .. -DCMAKE_INSTALL_PREFIX=${YUTOVO_DEPLOY} -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DCMAKE_C_COMPILER=gcc
CPATH="%{_builddir}/third_party/boost_1_83_0${CPATH:+:${CPATH}}" make %{?_smp_mflags}
make install

# Build mathgl
mkdir -p %{_builddir}/third_party/mathgl-8.0.3/build
cd %{_builddir}/third_party/mathgl-8.0.3/build
cmake .. \
    -DCMAKE_INSTALL_PREFIX=${YUTOVO_DEPLOY} \
    -DCMAKE_INSTALL_LIBDIR=${YUTOVO_DEPLOY} \
    -DCMAKE_BUILD_TYPE=Release \
    -Denable-openmp=False \
    -Denable-png=False \
    -Denable-opengl=False
make %{?_smp_mflags}
make install

# Build libharu
mkdir -p %{_builddir}/third_party/libharu-2.4.4/build
cd %{_builddir}/third_party/libharu-2.4.4/build
cmake .. \
    -DCMAKE_INSTALL_PREFIX=${YUTOVO_DEPLOY} \
    -DCMAKE_INSTALL_LIBDIR=${YUTOVO_DEPLOY}/lib \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_SHARED_LIBS=OFF
make %{?_smp_mflags}
make install
cd %{_builddir}

# Патч: заменить путь к stb_image
find %{_builddir}/submodules/yutovo-editor-%{version} -name "*.cpp" -o -name "*.h" | \
    xargs sed -i 's|<stb_image/|<stb/|g'
# GCC 13 in C++20 mode rejects the constructor-with-template-arguments syntax used by the giac headers (Tfraction<T> (...)); the editor uses no C++20
# features, so build it as C++17 like the other components
sed -i 's/^set(CMAKE_CXX_STANDARD 20)/set(CMAKE_CXX_STANDARD 17)/' %{_builddir}/submodules/yutovo-editor-%{version}/src/CMakeLists.txt

# Build yutovo-editor
mkdir -p %{_builddir}/submodules/yutovo-editor-%{version}/build
cd %{_builddir}/submodules/yutovo-editor-%{version}/build
cmake .. -DCMAKE_INSTALL_PREFIX=${YUTOVO_DEPLOY} -DCMAKE_BUILD_TYPE=Release -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DBUILD_TESTS=OFF
make %{?_smp_mflags}
make install

# Build yutovo-library (help pages; .yut.in templates go through cpp, ZIP
# gzips the resulting .yut files)
cd %{_builddir}/submodules/yutovo-library-1.1.1
mkdir -p ${YUTOVO_DEPLOY}/bin
./make_library.sh ${YUTOVO_DEPLOY}/bin ZIP

# Build main application
mkdir -p %{_builddir}/yutovo-desktop-%{version}/build
cd %{_builddir}/yutovo-desktop-%{version}/build
# Patch out static libstdc++ linking for proper distro packaging
sed -i 's/-static-libstdc++//g' \
    %{_builddir}/yutovo-desktop-%{version}/src/CMakeLists.txt
sed -i '/libstdc++.a/d' \
    %{_builddir}/yutovo-desktop-%{version}/src/CMakeLists.txt
cmake .. \
    -DCMAKE_INSTALL_PREFIX=%{_prefix} \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
    -DBUILD_TESTS=OFF \
    -DCMAKE_PREFIX_PATH=${YUTOVO_DEPLOY} \
    -DMathGL_DIR=${YUTOVO_DEPLOY}/lib/cmake/mathgl2
make %{?_smp_mflags}

%install
# Install the actual binary from the build tree
install -Dm755 %{_builddir}/%{name}-desktop-%{version}/build/src/%{name}-desktop \
    %{buildroot}%{_bindir}/%{name}-desktop

# The solver calculator worker must stay next to the application executable
install -Dm755 %{_builddir}/%{name}-deploy/bin/yutovo-solver-calculator-worker \
    %{buildroot}%{_bindir}/yutovo-solver-calculator-worker

# Create wrapper script expected by the desktop file
install -Dm755 /dev/stdin %{buildroot}%{_bindir}/%{name} <<'EOF'
#!/bin/bash
file="$1"
LOGS="${HOME}/.local/share/%{name}/log"
mkdir -p "$LOGS"
if [[ -n "$file" ]]; then
    %{_bindir}/%{name}-desktop "$file" --logs-path="$LOGS"
else
    %{_bindir}/%{name}-desktop --logs-path="$LOGS"
fi
EOF

# Desktop file
install -Dm644 %{_builddir}/%{name}-desktop-%{version}/setup/yutovo.desktop \
    %{buildroot}%{_datadir}/applications/yutovo.desktop
# The .desktop expects "run.sh", replace with our wrapper
sed -i 's|^Exec=run.sh|Exec=%{name}|' %{buildroot}%{_datadir}/applications/yutovo.desktop
# Fix icon name: remove extension per Icon Theme Specification
sed -i 's|^Icon=application-x-yutovo.png|Icon=yutovo|' %{buildroot}%{_datadir}/applications/yutovo.desktop

# MIME type
install -Dm644 %{_builddir}/%{name}-desktop-%{version}/setup/yutovo.xml \
    %{buildroot}%{_datadir}/mime/packages/yutovo.xml

# Icons
install -Dm644 %{_builddir}/%{name}-desktop-%{version}/setup/yutovo-16.png \
    %{buildroot}%{_datadir}/icons/hicolor/16x16/apps/yutovo.png
install -Dm644 %{_builddir}/%{name}-desktop-%{version}/setup/yutovo-32.png \
    %{buildroot}%{_datadir}/icons/hicolor/32x32/apps/yutovo.png
install -Dm644 %{_builddir}/%{name}-desktop-%{version}/setup/yutovo-64.png \
    %{buildroot}%{_datadir}/icons/hicolor/64x64/apps/yutovo.png
install -Dm644 %{_builddir}/%{name}-desktop-%{version}/setup/yutovo.png \
    %{buildroot}%{_datadir}/icons/hicolor/256x256/apps/yutovo.png

# AppData
install -Dm644 %{_builddir}/%{name}-desktop-%{version}/setup/com.yutovo.yutovo.metainfo.xml \
    %{buildroot}%{_datadir}/metainfo/com.yutovo.yutovo.metainfo.xml

# License and documentation
install -Dm644 %{_builddir}/%{name}-desktop-%{version}/LICENSE \
    %{buildroot}%{_datadir}/doc/%{name}-%{version}/LICENSE
install -Dm644 %{_builddir}/%{name}-desktop-%{version}/README.md \
    %{buildroot}%{_datadir}/doc/%{name}-%{version}/README.md

# Translations
mkdir -p %{buildroot}%{_datadir}/%{name}/translations
install -Dm644 %{_builddir}/%{name}-desktop-%{version}/build/src/*.qm \
    %{buildroot}%{_datadir}/%{name}/translations/

# Library files (drop a stray unpacked leftover shipped in the upstream tarball)
mkdir -p %{buildroot}%{_datadir}/%{name}
cp -r %{_builddir}/%{name}-deploy/bin/library %{buildroot}%{_datadir}/%{name}/
find %{buildroot}%{_datadir}/%{name}/library -name '*.tmp_unpacked' -delete

%check
desktop-file-validate %{buildroot}%{_datadir}/applications/yutovo.desktop
appstream-util validate-relax --nonet %{buildroot}%{_datadir}/metainfo/com.yutovo.yutovo.metainfo.xml || true

%post
update-desktop-database %{_datadir}/applications &> /dev/null || :
update-mime-database %{_datadir}/mime &> /dev/null || :

%postun
update-desktop-database %{_datadir}/applications &> /dev/null || :
update-mime-database %{_datadir}/mime &> /dev/null || :

%files
%doc %{_datadir}/doc/%{name}-%{version}/LICENSE
%doc %{_datadir}/doc/%{name}-%{version}/README.md
%{_bindir}/%{name}
%{_bindir}/%{name}-desktop
%{_bindir}/yutovo-solver-calculator-worker
%{_datadir}/applications/yutovo.desktop
%{_datadir}/mime/packages/yutovo.xml
%{_datadir}/icons/hicolor/*/apps/yutovo.png
%{_datadir}/metainfo/com.yutovo.yutovo.metainfo.xml
%{_datadir}/%{name}/

%changelog
* Mon Aug 31 2026 Denis Gordenin <denis@yutovo.com> - 1.7.1-alt1
- Update to 1.7.1: giac 2.0.0 replaces symengine, new yutovo-calculator 1.6.1,
  yutovo-solver 1.3.1, yutovo-library 1.1.1 (make_library.sh)
- Package the yutovo-solver-calculator-worker helper binary
- Use system gmp/mpfr/boost instead of bundled static builds

* Mon Jun 29 2026 Denis Gordenin <denis@yutovo.com> - 1.6.2-alt1
- Build for ALT Linux: symengine from source, adapted deps, fixed macros
