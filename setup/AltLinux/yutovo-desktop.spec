Name: yutovo
Version: 1.6.2
Release: alt1
Summary: Powerful visual calculator with graphical representation of mathematical formulas
Group: Sciences/Mathematics

License: GPL-3.0-only
URL: https://yutovo.com
Source0: https://github.com/denprog/yutovo-desktop/archive/refs/tags/v%{version}.tar.gz#/%{name}-desktop-%{version}.tar.gz

# Yutovo submodules
Source1: https://github.com/denprog/yutovo-logger/archive/refs/tags/v1.0.6.tar.gz#/yutovo-logger-1.0.6.tar.gz
Source2: https://github.com/denprog/yutovo-calculator/archive/refs/tags/v1.5.1.tar.gz#/yutovo-calculator-1.5.1.tar.gz
Source3: https://github.com/denprog/yutovo-solver/archive/refs/tags/v1.2.2.tar.gz#/yutovo-solver-1.2.2.tar.gz
Source4: https://github.com/denprog/yutovo-editor/archive/refs/tags/v%{version}.tar.gz#/yutovo-editor-%{version}.tar.gz
Source5: https://github.com/denprog/yutovo-library/archive/desktop.tar.gz#/yutovo-library-desktop.tar.gz
Source6: https://github.com/libharu/libharu/archive/refs/tags/v2.4.4.tar.gz#/libharu-2.4.4.tar.gz
Source7: https://github.com/symengine/symengine/archive/refs/tags/v0.14.0.tar.gz#/symengine-0.14.0.tar.gz
Source12: https://ftp.gnu.org/gnu/gmp/gmp-6.3.0.tar.xz
Source13: https://www.mpfr.org/mpfr-4.2.1/mpfr-4.2.1.tar.gz
Source14: https://ftp.gnu.org/gnu/mpc/mpc-1.3.1.tar.gz
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

Requires: qt5-qtbase-gui
Requires: hicolor-icon-theme

%description
Yutovo is a powerful calculator with graphical representation of mathematics
operations inside a text editor. Based on Qt and written in C++.

Features:
- Graphical representation of mathematical formulas
- Math WYSIWYG editor
- Arbitrary precision numbers
- Symbolic calculations (giac)
- Units and unit conversion
- Graphs of functions

%prep
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
tar -xzf %{SOURCE7} -C %{_builddir}/third_party
tar -xzf %{SOURCE17} -C %{_builddir}/third_party
tar -xJf %{SOURCE12} -C %{_builddir}/third_party
tar -xzf %{SOURCE13} -C %{_builddir}/third_party
tar -xzf %{SOURCE14} -C %{_builddir}/third_party

%build
export YUTOVO_DEPLOY=%{_builddir}/%{name}-deploy

# Save RPM flags and use clean flags for autotools-based third-party libs
# -std=gnu17 is required for GMP 6.3.0 with GCC 16 (C23 default)
# LDFLAGS is cleared to avoid PIE/hardening conflicts with static-only builds
export YUTOVO_THIRD_PARTY_CFLAGS="-O2 -g -std=gnu17 -fPIC"
export YUTOVO_THIRD_PARTY_LDFLAGS=""

# Build yutovo-logger
mkdir -p %{_builddir}/submodules/yutovo-logger-1.0.6/build
cd %{_builddir}/submodules/yutovo-logger-1.0.6/build
cmake .. -DCMAKE_INSTALL_PREFIX=${YUTOVO_DEPLOY} -DCMAKE_BUILD_TYPE=Release
make %{?_smp_mflags}
make install

# Build gmp
cd %{_builddir}/third_party/gmp-6.3.0
CFLAGS="${YUTOVO_THIRD_PARTY_CFLAGS}" LDFLAGS="${YUTOVO_THIRD_PARTY_LDFLAGS}" ./configure --enable-static --disable-shared --prefix=${YUTOVO_DEPLOY}
CFLAGS="${YUTOVO_THIRD_PARTY_CFLAGS}" LDFLAGS="${YUTOVO_THIRD_PARTY_LDFLAGS}" make %{?_smp_mflags}
make install

# Build mpfr
cd %{_builddir}/third_party/mpfr-4.2.1
CFLAGS="${YUTOVO_THIRD_PARTY_CFLAGS}" LDFLAGS="${YUTOVO_THIRD_PARTY_LDFLAGS}" ./configure --enable-static --disable-shared --prefix=${YUTOVO_DEPLOY} --with-gmp=${YUTOVO_DEPLOY}
CFLAGS="${YUTOVO_THIRD_PARTY_CFLAGS}" LDFLAGS="${YUTOVO_THIRD_PARTY_LDFLAGS}" make %{?_smp_mflags}
make install

# Build mpc
cd %{_builddir}/third_party/mpc-1.3.1
CFLAGS="${YUTOVO_THIRD_PARTY_CFLAGS}" LDFLAGS="${YUTOVO_THIRD_PARTY_LDFLAGS}" ./configure --enable-static --disable-shared --prefix=${YUTOVO_DEPLOY} --with-gmp=${YUTOVO_DEPLOY} --with-mpfr=${YUTOVO_DEPLOY}
CFLAGS="${YUTOVO_THIRD_PARTY_CFLAGS}" LDFLAGS="${YUTOVO_THIRD_PARTY_LDFLAGS}" make %{?_smp_mflags}
make install

# Build symengine (from flatpak config)
mkdir -p %{_builddir}/third_party/symengine-0.14.0/build
cd %{_builddir}/third_party/symengine-0.14.0/build
cmake .. \
    -DCMAKE_INSTALL_PREFIX=${YUTOVO_DEPLOY} \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_TESTING=OFF \
    -DBUILD_BENCHMARKS=OFF \
    -DWITH_MPFR=True \
    -DWITH_MPC=True \
    -DBUILD_SHARED_LIBS=False \
    -DGMP_INCLUDE_DIR=${YUTOVO_DEPLOY}/include \
    -DGMP_LIBRARY=${YUTOVO_DEPLOY}/lib/libgmp.a \
    -DMPFR_INCLUDE_DIR=${YUTOVO_DEPLOY}/include \
    -DMPFR_LIBRARY=${YUTOVO_DEPLOY}/lib/libmpfr.a \
    -DMPC_INCLUDE_DIR=${YUTOVO_DEPLOY}/include \
    -DMPC_LIBRARY=${YUTOVO_DEPLOY}/lib/libmpc.a
make %{?_smp_mflags}
make install

# Build yutovo-calculator
mkdir -p %{_builddir}/submodules/yutovo-calculator-1.5.1/build
cd %{_builddir}/submodules/yutovo-calculator-1.5.1/build
cmake .. -DCMAKE_INSTALL_PREFIX=${YUTOVO_DEPLOY} -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DCMAKE_C_COMPILER=gcc
make %{?_smp_mflags}
make install

# Build yutovo-solver
mkdir -p %{_builddir}/submodules/yutovo-solver-1.2.2/build
cd %{_builddir}/submodules/yutovo-solver-1.2.2/build
cmake .. -DCMAKE_INSTALL_PREFIX=${YUTOVO_DEPLOY} -DCMAKE_BUILD_TYPE=Release -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DCMAKE_C_COMPILER=gcc
make %{?_smp_mflags}
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

# Build yutovo-editor
mkdir -p %{_builddir}/submodules/yutovo-editor-%{version}/build
cd %{_builddir}/submodules/yutovo-editor-%{version}/build
cmake .. -DCMAKE_INSTALL_PREFIX=${YUTOVO_DEPLOY} -DCMAKE_BUILD_TYPE=Release -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DBUILD_TESTS=OFF
make %{?_smp_mflags}
make install

# Build yutovo-library
cd %{_builddir}/submodules/yutovo-library-desktop
./pack.sh
mkdir -p ${YUTOVO_DEPLOY}/bin
cp -r library ${YUTOVO_DEPLOY}/bin/

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

# Library files
mkdir -p %{buildroot}%{_datadir}/%{name}
cp -r %{_builddir}/%{name}-deploy/bin/library %{buildroot}%{_datadir}/%{name}/

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
%{_datadir}/applications/yutovo.desktop
%{_datadir}/mime/packages/yutovo.xml
%{_datadir}/icons/hicolor/*/apps/yutovo.png
%{_datadir}/metainfo/com.yutovo.yutovo.metainfo.xml
%{_datadir}/%{name}/

%changelog
* Mon Jun 29 2026 Denis Gordenin <denis@yutovo.com> - 1.6.2-alt1
- Build for ALT Linux: symengine from source, adapted deps, fixed macros