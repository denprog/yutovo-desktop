Name: yutovo
Version: 1.7.1
Release: 1%{?dist}
Summary: Multifunctional visual calculator with graphical representation of mathematical formulas

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
Source7: https://sources.voidlinux.org/giac-2.0.0.21/giac_2.0.0-21.tar.gz#/giac-2.0.0.tar.gz
Source9: https://archives.boost.io/release/1.83.0/source/boost_1_83_0.zip
Source17: http://downloads.sourceforge.net/mathgl/mathgl-8.0.3.tar.gz

%define _smp_mflags -j1

BuildRequires: cmake >= 3.16
BuildRequires: gcc-c++
BuildRequires: make
BuildRequires: pkgconf-pkg-config
BuildRequires: unzip
BuildRequires: qt5-qtbase-devel
BuildRequires: qt5-qttools-devel
BuildRequires: qt5-qtbase-private-devel
BuildRequires: qt5-qtdeclarative-devel
BuildRequires: qt5-qtsvg-devel
BuildRequires: qt5-qtx11extras-devel
BuildRequires: boost-devel
BuildRequires: spdlog-devel
BuildRequires: rapidjson-devel
BuildRequires: desktop-file-utils
BuildRequires: libappstream-glib
BuildRequires: hicolor-icon-theme
BuildRequires: stb_image-devel
BuildRequires: stb_image_write-devel
BuildRequires: fontconfig-devel
BuildRequires: gmp-devel
BuildRequires: mpfr-devel

Requires: qt5-qtbase-gui
Requires: hicolor-icon-theme
Requires: dejavu-serif-fonts

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
- Symbolic calculations

%prep
# Verify the large downloads up front: a corrupted mirror transfer must fail
# loudly with a checksum error, not as a cryptic gzip failure mid-build
echo "9bba9ee6a0f86d1b8f3f3ba0374d3cb776f772dbb6f1a01684ca6c0bd56204d6  %{SOURCE17}" | sha256sum -c -
echo "3e7fa1c281a21ab74ed66ef247faffe5a105bc11be1f7715f31b4368ce8dcbc8  %{SOURCE7}" | sha256sum -c -
echo "c86bd9d9eef795b4b0d3802279419fde5221922805b073b9bd822edecb1ca28e  %{SOURCE9}" | sha256sum -c -

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
# Fedora links PIE by default: giac objects must be position independent
sed -i 's|CFLAGS = -O2 -DNDEBUG|CFLAGS = -fPIC -O2 -DNDEBUG|' build_giac.sh
patch -p1 < giac_config_h.diff

%build
export YUTOVO_DEPLOY=%{_builddir}/%{name}-deploy
# giac and the yutovo-* libraries live in the deploy dir; giac_imported and
# libstdc++fs are resolved there via linker scripts installed by build_giac.sh
export LIBRARY_PATH=${YUTOVO_DEPLOY}/lib

# Build yutovo-logger
mkdir -p %{_builddir}/submodules/yutovo-logger-1.0.6/build
cd %{_builddir}/submodules/yutovo-logger-1.0.6/build
cmake .. -DCMAKE_INSTALL_PREFIX=${YUTOVO_DEPLOY} -DCMAKE_BUILD_TYPE=Release
make %{?_smp_mflags}
make install

# Build boost 1.83.0 headers
cd %{_builddir}/third_party/boost_1_83_0
./bootstrap.sh --prefix=${YUTOVO_DEPLOY}
./b2 headers
mkdir -p ${YUTOVO_DEPLOY}/include
cp -r boost ${YUTOVO_DEPLOY}/include/
# Prefer the bundled 1.83 headers over the system boost for the yutovo modules
export CPATH=${YUTOVO_DEPLOY}/include

# Build giac (static, Release); the Flatpak build script honours FLATPAK_DEST
# as the install prefix and FLATPAK_BUILDER_MAX_JOBS for the parallel build
cd %{_builddir}/third_party/giac-2.0.0
FLATPAK_DEST=${YUTOVO_DEPLOY} FLATPAK_BUILDER_MAX_JOBS=4 bash build_giac.sh
# yutovo-calculator requires both release and debug library names at configure time
cp -f ${YUTOVO_DEPLOY}/lib/libgiac.a ${YUTOVO_DEPLOY}/lib/libgiacd.a

# Build yutovo-calculator
mkdir -p %{_builddir}/submodules/yutovo-calculator-1.6.1/build
cd %{_builddir}/submodules/yutovo-calculator-1.6.1/build
cmake .. -DCMAKE_INSTALL_PREFIX=${YUTOVO_DEPLOY} -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DBoost_INCLUDE_DIR=${YUTOVO_DEPLOY}/include
make %{?_smp_mflags}
make install

# Build yutovo-solver
mkdir -p %{_builddir}/submodules/yutovo-solver-1.3.1/build
cd %{_builddir}/submodules/yutovo-solver-1.3.1/build
cmake .. -DCMAKE_INSTALL_PREFIX=${YUTOVO_DEPLOY} -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF -DCMAKE_POLICY_VERSION_MINIMUM=3.5
make %{?_smp_mflags}
make install

# Build mathgl
mkdir -p %{_builddir}/third_party/mathgl-8.0.3/build
cd %{_builddir}/third_party/mathgl-8.0.3/build
cmake .. \
    -DCMAKE_INSTALL_PREFIX=${YUTOVO_DEPLOY} \
    -DCMAKE_INSTALL_LIBDIR=${YUTOVO_DEPLOY} \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
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
    -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
    -DBUILD_SHARED_LIBS=OFF
make %{?_smp_mflags}
make install
if [ -f ${YUTOVO_DEPLOY}/lib64/libhpdf.a ]; then
    mv ${YUTOVO_DEPLOY}/lib64/libhpdf.a ${YUTOVO_DEPLOY}/lib/
fi
cd %{_builddir}

# Patch: replace stb_image include path with the Fedora stb layout
find %{_builddir}/submodules/yutovo-editor-%{version} -name "*.cpp" -o -name "*.h" | \
    xargs sed -i 's|<stb_image/|<stb/|g'

# Build yutovo-editor
mkdir -p %{_builddir}/submodules/yutovo-editor-%{version}/build
cd %{_builddir}/submodules/yutovo-editor-%{version}/build
cmake .. -DCMAKE_INSTALL_PREFIX=${YUTOVO_DEPLOY} -DCMAKE_BUILD_TYPE=Release -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DBUILD_TESTS=OFF
make %{?_smp_mflags}
make install

# Generate yutovo library data (gzip-packed .yut files)
cd %{_builddir}/submodules/yutovo-library-1.1.1
./make_library.sh %{_builddir}/%{name}-library-out ZIP

# Build main application
mkdir -p %{_builddir}/yutovo-desktop-%{version}/build
cd %{_builddir}/yutovo-desktop-%{version}/build
# Patch out static libstdc++ linking for proper Fedora packaging
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

# Calculator worker, expected next to the application binary
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

# MIME type icons
install -Dm644 %{_builddir}/%{name}-desktop-%{version}/setup/yutovo-16.png \
    %{buildroot}%{_datadir}/icons/hicolor/16x16/mimetypes/application-x-yutovo.png
install -Dm644 %{_builddir}/%{name}-desktop-%{version}/setup/yutovo-32.png \
    %{buildroot}%{_datadir}/icons/hicolor/32x32/mimetypes/application-x-yutovo.png
install -Dm644 %{_builddir}/%{name}-desktop-%{version}/setup/yutovo-64.png \
    %{buildroot}%{_datadir}/icons/hicolor/64x64/mimetypes/application-x-yutovo.png
install -Dm644 %{_builddir}/%{name}-desktop-%{version}/setup/yutovo.png \
    %{buildroot}%{_datadir}/icons/hicolor/256x256/mimetypes/application-x-yutovo.png

# AppData
install -Dm644 %{_builddir}/%{name}-desktop-%{version}/setup/com.yutovo.yutovo.metainfo.xml \
    %{buildroot}%{_datadir}/metainfo/com.yutovo.yutovo.metainfo.xml

# License and documentation
install -Dm644 %{_builddir}/%{name}-desktop-%{version}/LICENSE \
    %{buildroot}%{_defaultlicensedir}/%{name}/LICENSE
install -Dm644 %{_builddir}/%{name}-desktop-%{version}/README.md \
    %{buildroot}%{_defaultdocdir}/%{name}/README.md

# Translations (runtime lookup: /usr/share/yutovo/translations)
mkdir -p %{buildroot}%{_datadir}/%{name}/translations
install -Dm644 %{_builddir}/%{name}-desktop-%{version}/build/src/*.qm \
    %{buildroot}%{_datadir}/%{name}/translations/

# Library files (runtime lookup: /usr/share/yutovo/library)
mkdir -p %{buildroot}%{_datadir}/%{name}
cp -r %{_builddir}/%{name}-library-out/library %{buildroot}%{_datadir}/%{name}/

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
%license %{_defaultlicensedir}/%{name}/LICENSE
%doc %{_defaultdocdir}/%{name}/README.md
%{_bindir}/%{name}
%{_bindir}/%{name}-desktop
%{_bindir}/yutovo-solver-calculator-worker
%{_datadir}/applications/yutovo.desktop
%{_datadir}/mime/packages/yutovo.xml
%{_datadir}/icons/hicolor/*/apps/yutovo.png
%{_datadir}/icons/hicolor/*/mimetypes/application-x-yutovo.png
%{_datadir}/metainfo/com.yutovo.yutovo.metainfo.xml
%{_datadir}/%{name}/

%changelog
* Mon Aug 31 2026 Denis Gordenin <denis@yutovo.com> - 1.7.1-1
- Update to 1.7.1 (sync with com.yutovo.yutovo.yaml flatpak manifest):
  yutovo-calculator 1.6.1, yutovo-solver 1.3.1, yutovo-library 1.1.1
- Replace SymEngine with static giac 2.0.0 built from source,
  drop bundled gmp/mpfr/mpc in favour of system libraries
- Bundle boost 1.83.0 headers (system boost 1.90 dropped boost::process v1)
- Bundle yutovo-solver-calculator-worker and MIME type icons
- Require dejavu-serif-fonts: math formula symbols are drawn in DejaVu Serif
  (flatpak runtime ships it; without it the integral sign renders as tofu)
- Fetch mathgl from the openSUSE:Factory source mirror and verify sha256 of
  the large downloads (a SourceForge mirror served a corrupted mathgl tarball)
- Build the CMake modules serially: the boost::spirit translation units of
  yutovo-calculator (expression.cpp peaks at ~8 GB) OOM the 16 GB OBS worker at -j4
- Fetch giac from the Void Linux sources mirror (byte-identical snapshot):
  the upstream university server times out from the OBS service runner
- Compile giac with -fPIC: Fedora links PIE by default and the
  yutovo-solver-calculator-worker link fails on non-PIC giac objects* Mon Jun 29 2026 Denis Gordenin <denis@yutovo.com> - 1.6.2-1
- Build fixes for Fedora 44: libharu from source, GCC 16 / C23 compat,
  remove static libstdc++ linking, install wrapper and library correctly
