Name: mathgl
Version: 8.0.3
Release: alt1
Summary: Cross-platform library for high quality scientific graphics
Group: Sciences/Mathematics

License: GPLv3
URL: http://mathgl.sourceforge.net
Source0: http://downloads.sourceforge.net/mathgl/mathgl-%{version}.tar.gz

BuildRequires: cmake >= 3.12
BuildRequires: gcc-c++
BuildRequires: libpng-devel
BuildRequires: zlib-devel

%description
MathGL is a cross-platform library for making high-quality scientific
graphics under Linux and Windows. It provides more than 70 general-purpose
"pure" (not connected with any GUI) plotting functions for 1D, 2D and 3D
data, contour and vector fields, flow and stream lines, pie charts, and
supports PNG, GIF, JPEG and SVG output (via the enabled formats).

This package contains the command-line conversion utilities.

%package -n libmgl8
Summary: MathGL shared library
Group: Sciences/Mathematics
Requires: mathgl-fonts
%description -n libmgl8
Shared library of MathGL, a cross-platform library for making
high-quality scientific graphics.

%package -n libmgl-devel
Summary: Development files for MathGL
Group: Sciences/Mathematics
Requires: libmgl8 = %{version}-%{release}
Requires: libpng-devel
Requires: zlib-devel
%description -n libmgl-devel
Development files (headers, CMake import configs) for MathGL,
a cross-platform library for making high-quality scientific graphics.

%package -n mathgl-fonts
Summary: MathGL font files
Group: Sciences/Mathematics
%description -n mathgl-fonts
Bitmap font files used by MathGL for text rendering
(Latin, Cyrillic, Greek and symbols).

%package -n mathgl-fonts-cjk
Summary: MathGL font files for CJK scripts
Group: Sciences/Mathematics
%description -n mathgl-fonts-cjk
Large bitmap font files used by MathGL for Chinese, Japanese and
Korean text rendering (about 0.5 GB installed).

%prep
%setup

%build
cmake -B build \
    -DCMAKE_INSTALL_PREFIX=%{_prefix} \
    -DMathGL_INSTALL_LIB_DIR=%{_libdir} \
    -DCMAKE_BUILD_TYPE=Release \
    -Denable-opengl=False \
    -Denable-openmp=False \
    -Denable-mpi=False \
    -Denable-all-docs=False \
    -Denable-all-swig=False \
    -Denable-mgltex=False
cmake --build build --parallel

%install
make -C build install DESTDIR=%{buildroot}

# The web CGI demo is not meant for /usr/bin
rm -f %{buildroot}%{_bindir}/mgl.cgi

%files
%doc COPYING COPYING_LGPL ChangeLog.txt AUTHORS
%{_bindir}/mglconv
%{_bindir}/mgltask

%files -n libmgl8
%{_libdir}/libmgl.so.8*

%files -n mathgl-fonts
%dir %{_datadir}/mathgl
%{_datadir}/mathgl/fonts
%exclude %{_datadir}/mathgl/fonts/Noto*CJK.vfmb

%files -n mathgl-fonts-cjk
%{_datadir}/mathgl/fonts/Noto*CJK.vfmb

%files -n libmgl-devel
%{_includedir}/mgl2/
%{_libdir}/libmgl.so
%{_libdir}/cmake/mathgl2/

%changelog
* Thu Oct 16 2026 Denis Gordenin <denis@yutovo.com> - 8.0.3-alt1
- Initial build for ALT Sisyphus
