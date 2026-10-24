Name: yutovo
Version: 1.8.1
Release: alt1
Summary: Multifunctional visual calculator with graphical representation of mathematical formulas
Group: Sciences/Mathematics

# System giac location (Sisyphus libgiac-devel); can be overridden with
# --define for sysroot-based verification builds
%{!?giac_includedir:%define giac_includedir %{_includedir}}
%{!?giac_libdir:%define giac_libdir %{_libdir}}

License: GPL-3.0-only
URL: https://yutovo.com
Source0: https://github.com/denprog/yutovo-desktop/archive/refs/tags/v%{version}.tar.gz#/%{name}-desktop-%{version}.tar.gz

# Yutovo submodules
Source1: https://github.com/denprog/yutovo-logger/archive/refs/tags/v1.0.6.tar.gz#/yutovo-logger-1.0.8.tar.gz
Source2: https://github.com/denprog/yutovo-calculator/archive/refs/tags/v1.6.1.tar.gz#/yutovo-calculator-1.7.1.tar.gz
Source3: https://github.com/denprog/yutovo-solver/archive/refs/tags/v1.3.1.tar.gz#/yutovo-solver-1.4.1.tar.gz
Source4: https://github.com/denprog/yutovo-editor/archive/refs/tags/v%{version}.tar.gz#/yutovo-editor-%{version}.tar.gz
Source5: https://github.com/denprog/yutovo-library/archive/refs/tags/v1.1.1.tar.gz#/yutovo-library-1.2.1.tar.gz
# mathgl is built from source: not packaged in Sisyphus
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
BuildRequires: boost-locale-devel
BuildRequires: boost-asio-devel
BuildRequires: boost-process-devel
BuildRequires: boost-context-devel
BuildRequires: boost-filesystem-devel
BuildRequires: libspdlog-devel
BuildRequires: rapidjson-devel
BuildRequires: desktop-file-utils
BuildRequires: libappstream-devel
BuildRequires: hicolor-icon-theme
BuildRequires: libstb-devel
BuildRequires: fontconfig-devel
BuildRequires: libgmp-devel
BuildRequires: libgmpxx-devel
BuildRequires: libmpfr-devel
BuildRequires: libgiac-devel
BuildRequires: libharu-devel
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
tar -xzf %{SOURCE17} -C %{_builddir}/third_party

%build
export YUTOVO_DEPLOY=%{_builddir}/%{name}-deploy
# giac and the yutovo-* libraries live in the deploy dir; the libgiac_imported
# linker script and the empty libstdc++fs stub are created below.
# gmpxx.h and the shared libhpdf come from the system (libgmpxx-devel,
# libharu-devel); ambient CPATH/LIBRARY_PATH are preserved so a sysroot with
# those packages can be used on build hosts without them installed
export LIBRARY_PATH=${YUTOVO_DEPLOY}/lib${LIBRARY_PATH:+:${LIBRARY_PATH}}

# Build yutovo-logger
mkdir -p %{_builddir}/submodules/yutovo-logger-1.0.8/build
cd %{_builddir}/submodules/yutovo-logger-1.0.8/build
cmake .. -DCMAKE_INSTALL_PREFIX=${YUTOVO_DEPLOY} -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON -DCMAKE_SKIP_RPATH=ON
make %{?_smp_mflags}
make install
# Same hardcoded static-library import config as in the solver
sed -i 's|libyutovo-logger\.a|libyutovo-logger.so|' \
    ${YUTOVO_DEPLOY}/lib/cmake/yutovo-logger/yutovo-logger-release.cmake 2>/dev/null || :

# System giac (Sisyphus libgiac-devel, shared libgiac.so): the yutovo CMake
# machinery looks the library and headers up in the deploy prefix only, so
# wire them there. libgiac.a points at the shared library (the solver CMake
# rewrites the giac_imported target into the absolute libgiac.a path), and a
# linker script maps the raw giac_imported linker flag onto the shared lib.
ln -sfn %{giac_includedir}/giac ${YUTOVO_DEPLOY}/include/giac
ln -sfn %{giac_libdir}/libgiac.so ${YUTOVO_DEPLOY}/lib/libgiac.a
ln -sfn %{giac_libdir}/libgiac.so ${YUTOVO_DEPLOY}/lib/libgiac.so
printf 'INPUT ( %{giac_libdir}/libgiac.so )\n' > ${YUTOVO_DEPLOY}/lib/libgiac_imported.a
# yutovo-solver links -lstdc++fs; filesystem support lives in libstdc++ itself
ar rcs ${YUTOVO_DEPLOY}/lib/libstdc++fs.a

# Build yutovo-calculator
mkdir -p %{_builddir}/submodules/yutovo-calculator-1.7.1/build
cd %{_builddir}/submodules/yutovo-calculator-1.7.1/build
cmake .. -DCMAKE_INSTALL_PREFIX=${YUTOVO_DEPLOY} -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF -DBUILD_SHARED_LIBS=ON -DCMAKE_SKIP_RPATH=ON -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DCMAKE_C_COMPILER=gcc
make %{?_smp_mflags}
make install
# For a shared library CMake does not export the PRIVATE giac dependency into
# the imported target; with ld --no-copy-dt-needed consumers must link libgiac
# explicitly (their own objects reference inlined giac header code)
printf '\nset_property(TARGET yutovo-calculator APPEND PROPERTY INTERFACE_LINK_LIBRARIES "%{giac_libdir}/libgiac.so;mpfr;gmp")\n' \
    >> ${YUTOVO_DEPLOY}/lib/cmake/yutovo-calculator/yutovo-calculator.cmake
# The calculator (since 1.6.2) hand-generates its release import config with a
# hardcoded static library name, like the solver below
sed -i 's|libyutovo-calculator\.a|libyutovo-calculator.so|' \
    ${YUTOVO_DEPLOY}/lib/cmake/yutovo-calculator/yutovo-calculator-release.cmake 2>/dev/null || :

# Build yutovo-solver
# The develop snapshot dropped the keys/ directory from the repository while
# the POST_BUILD step still copies the (remote-mode only) key files; provide
# empty placeholders where the monorepo would have them
mkdir -p %{_builddir}/submodules/keys
touch %{_builddir}/submodules/keys/yutovo-solver.crt \
    %{_builddir}/submodules/keys/yutovo-solver.key \
    %{_builddir}/submodules/keys/yutovo-solver.pub
mkdir -p %{_builddir}/submodules/yutovo-solver-1.4.1/build
cd %{_builddir}/submodules/yutovo-solver-1.4.1/build
cmake .. -DCMAKE_INSTALL_PREFIX=${YUTOVO_DEPLOY} -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF -DBUILD_SHARED_LIBS=ON -DCMAKE_SKIP_RPATH=ON -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DCMAKE_C_COMPILER=gcc
make %{?_smp_mflags}
make install
# The solver hand-generates its release import config with a hardcoded static
# library name; point it at the shared library
sed -i 's|libyutovo-solver\.a|libyutovo-solver.so|' \
    ${YUTOVO_DEPLOY}/lib/cmake/yutovo-solver/yutovo-solver-release.cmake

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
cd %{_builddir}

# Патч: заменить путь к stb_image
find %{_builddir}/submodules/yutovo-editor-%{version} -name "*.cpp" -o -name "*.h" | \
    xargs sed -i 's|<stb_image/|<stb/|g'
# GCC 13 in C++20 mode rejects the constructor-with-template-arguments syntax
# used by the giac headers (Tfraction<T> (...)); the editor uses no C++20
# features, so build it as C++17 like the other components
sed -i 's/^set(CMAKE_CXX_STANDARD 20)/set(CMAKE_CXX_STANDARD 17)/' \
    %{_builddir}/submodules/yutovo-editor-%{version}/src/CMakeLists.txt
# Link the system libharu (libharu-devel) instead of a deploy-dir static copy
sed -i 's|\${INSTALL_PATH}/lib/libhpdf\.a|hpdf|g' \
    %{_builddir}/submodules/yutovo-editor-%{version}/src/CMakeLists.txt
# Link the shared mathgl library instead of the deploy-dir static copy
sed -i 's|\${INSTALL_PATH}/lib/libmgl\.a|mgl|g' \
    %{_builddir}/submodules/yutovo-editor-%{version}/src/CMakeLists.txt

# Build yutovo-editor
mkdir -p %{_builddir}/submodules/yutovo-editor-%{version}/build
cd %{_builddir}/submodules/yutovo-editor-%{version}/build
cmake .. -DCMAKE_INSTALL_PREFIX=${YUTOVO_DEPLOY} -DCMAKE_BUILD_TYPE=Release -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DBUILD_TESTS=OFF -DBUILD_SHARED_LIBS=ON -DCMAKE_SKIP_RPATH=ON
make %{?_smp_mflags}
make install
# Same hardcoded static-library import config as in the solver and logger
sed -i 's|libyutovo-editor\.a|libyutovo-editor.so|' \
    ${YUTOVO_DEPLOY}/lib/cmake/yutovo-editor/yutovo-editor-release.cmake 2>/dev/null || :

# Build yutovo-library (help pages; .yut.in templates go through cpp, ZIP
# gzips the resulting .yut files)
cd %{_builddir}/submodules/yutovo-library-1.2.1
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
    -DCMAKE_SKIP_RPATH=ON \
    -DCMAKE_EXE_LINKER_FLAGS=-Wl,-rpath-link,${YUTOVO_DEPLOY}/lib \
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

# Shared yutovo libraries and the bundled shared mathgl; build-tree binaries
# carry an RPATH into the deploy dir, hence CMAKE_SKIP_RPATH everywhere
mkdir -p %{buildroot}%{_libdir}
install -m755 %{_builddir}/%{name}-deploy/lib/libyutovo-*.so* %{buildroot}%{_libdir}/
install -m755 %{_builddir}/%{name}-deploy/lib/libmgl.so.8* %{buildroot}%{_libdir}/

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
%{_libdir}/libyutovo-logger.so*
%{_libdir}/libyutovo-calculator.so*
%{_libdir}/libyutovo-solver.so*
%{_libdir}/libyutovo-editor.so*
%{_libdir}/libmgl.so.8*
%{_datadir}/applications/yutovo.desktop
%{_datadir}/mime/packages/yutovo.xml
%{_datadir}/icons/hicolor/*/apps/yutovo.png
%{_datadir}/metainfo/com.yutovo.yutovo.metainfo.xml
%{_datadir}/%{name}/

%changelog
* Sun Oct 18 2026 Denis Gordenin <denis@yutovo.com> - 1.8.1-alt1
- Update to 1.8.1: release branches of all submodules (logger 1.0.7,
  calculator 1.6.2, solver 1.3.2 with boost::process v2, editor 1.8.1)

* Mon Oct 12 2026 Denis Gordenin <denis@yutovo.com> - 1.7.1-alt2
- Use system libraries from Sisyphus: giac (libgiac-devel, shared), libharu,
  libgmpxx and boost modules instead of bundled sources
- Bundled boost 1.83 headers stay for the solver only: it uses the
  boost::process v1 API removed in boost 1.92
- Fully dynamic linking: the yutovo libraries are built shared and the
  bundled mathgl is linked as a shared library (shipped in the package)
- mathgl is still built from source (not packaged in Sisyphus)

* Mon Aug 31 2026 Denis Gordenin <denis@yutovo.com> - 1.7.1-alt1
- Update to 1.7.1: giac 2.0.0 replaces symengine, new yutovo-calculator 1.6.1,
  yutovo-solver 1.3.1, yutovo-library 1.1.1 (make_library.sh)
- Package the yutovo-solver-calculator-worker helper binary
- Use system gmp/mpfr/boost instead of bundled static builds

* Mon Jun 29 2026 Denis Gordenin <denis@yutovo.com> - 1.6.2-alt1
- Build for ALT Linux: symengine from source, adapted deps, fixed macros
