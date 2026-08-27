#!/bin/bash
# Builds the static giac 2.0.0 library for the Snap build environment (Release only).
# Mirrors third-party/build_giac_x64.sh from the yutovo monorepo: config.h is
# pre-patched by giac_config_h.diff (applied in override-pull) and the
# GUI/CLI/standalone modules are excluded.
# Usage: build_giac.sh <giac-source-dir> <install-prefix>
set -e

SRC="$(cd "$1" && pwd)"
APP_PREFIX="$2"
JOBS="${GIAC_BUILD_JOBS:-8}"

# The source may be flattened into the part source dir or keep its giac-2.0.0 top directory.
if [ ! -f "$SRC/src/giac.h" ] && [ -f "$SRC/giac-2.0.0/src/giac.h" ]; then
    SRC="$SRC/giac-2.0.0"
fi

if [ ! -f "$SRC/src/giac.h" ]; then
    echo "giac src directory not found: $SRC" >&2
    exit 1
fi

SRCDIR_ABS="$SRC/src"
BUILD_OBJDIR="$SRC/build/objs/giac-snap"
LIB="$SRC/build/libgiac_snap.a"

# Exclude GUI/CLI/plotting-GL/standalone modules.  Core math modules are kept.
EXCLUDE='\
aide.cc \
cas2.cc \
cas2html.cc \
casce.cc \
casctrl.cc \
Cfg.cc \
Editeur.cc \
Equation.cc \
factor.cc \
find_global_var.cc \
Flv_CStyle.cc \
Flv_Data_Source.cc \
Flve_Check_Button.cc \
Flve_Combo.cc \
Flve_Input.cc \
Flv_List.cc \
Flv_Style.cc \
Flv_Table.cc \
giacnspire.cc \
Graph.cc \
Graph3d.cc \
Help1.cc \
History.cc \
icas.cc \
Input.cc \
integrate.cc \
kadd.cc \
kdisplay.cc \
luabridge.cc \
lpsolve.cc \
markup.cc \
mkjs.cc \
normalize.cc \
opengl.cc \
partfrac.cc \
Print.cc \
renee.cc \
softmath.cc \
Tableur.cc \
translate.cc \
Xcas1.cc \
xcas.cc \
xcasce.cc \
xcasctrl.cc \
'

mkdir -p "$BUILD_OBJDIR"

cat > "$BUILD_OBJDIR/Makefile" <<EOF
SRCDIR = $SRCDIR_ABS
OBJDIR = $BUILD_OBJDIR
CXX = g++
DEFS = -DIN_GIAC -DGIAC_GENERIC_CONSTANTS -DHAVE_CONFIG_H -DGIAC_GGB -DTIMEOUT -DVERSION=\"2.0.0\" -DHAVE_NO_HOME_DIRECTORY -DSIZEOF_VOID_P=8 -DHAVE_LIBPTHREAD -DHAVE_SYS_TIMES_H -DHAVE_UNISTD_H -DHAVE_SYS_TIME_H -DHAVE_SYSCONF -DHAVE_LIBMPFR
INCLUDES = -I\$(SRCDIR)
CFLAGS = -O2 -DNDEBUG -fno-strict-aliasing -std=c++17 -pthread -fexceptions

EXCLUDE = $EXCLUDE
ALL_SOURCES := \$(wildcard \$(SRCDIR)/*.cc)
SOURCES := \$(filter-out \$(addprefix \$(SRCDIR)/,\$(EXCLUDE)),\$(ALL_SOURCES))
OBJECTS := \$(patsubst \$(SRCDIR)/%.cc,\$(OBJDIR)/%.o,\$(SOURCES))

.PHONY: all clean

all: \$(OBJECTS)

\$(OBJDIR)/%.o: \$(SRCDIR)/%.cc
	\$(CXX) \$(DEFS) \$(INCLUDES) \$(CFLAGS) -c \$< -o \$@
EOF

make -C "$BUILD_OBJDIR" -j"$JOBS"

rm -f "$LIB"
ar rcs "$LIB" "$BUILD_OBJDIR"/*.o

mkdir -p "$APP_PREFIX/lib" "$APP_PREFIX/include/giac"
install -Dm644 "$LIB" "$APP_PREFIX/lib/libgiac.a"
cp "$SRCDIR_ABS"/*.h "$APP_PREFIX/include/giac/"

# yutovo-solver v1.3.1 with BUILD_TESTS=OFF passes the raw name giac_imported to
# the linker; a linker script maps it to the real library (resolved via
# LIBRARY_PATH in the solver part).  The libstdc++fs stub covers -lstdc++fs.
ar rcs "$APP_PREFIX/lib/libstdc++fs.a"
printf 'INPUT ( libgiac.a )\n' > "$APP_PREFIX/lib/libgiac_imported.a"

echo "Giac 2.0.0 static library installed to: $APP_PREFIX/lib/libgiac.a"
