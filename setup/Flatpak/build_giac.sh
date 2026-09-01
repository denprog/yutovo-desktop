#!/bin/bash
# Builds the static giac 2.0.0 library inside the Flatpak sandbox (Release only).
# Mirrors third-party/build_giac_x64.sh from the yutovo monorepo: config.h is
# pre-patched by giac_config_h.diff and the GUI/CLI/standalone modules are excluded.
set -e

APP_PREFIX="${FLATPAK_DEST:-/app}"
BUILD_OBJDIR="$(pwd)/objs/giac-flatpak"
LIB="$(pwd)/libgiac.a"

SRCDIR_ABS=""
for d in ./src ./giac-*/src; do
    if [ -f "$d/giac.h" ]; then
        SRCDIR_ABS="$(cd "$d" && pwd)"
        break
    fi
done
if [ -z "$SRCDIR_ABS" ]; then
    echo "giac src directory not found" >&2
    exit 1
fi

EXTRA_INCLUDES=""
if [ -d /usr/include/x86_64-linux-gnu ]; then
    EXTRA_INCLUDES="-I/usr/include/x86_64-linux-gnu"
fi

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
INCLUDES = -I\$(SRCDIR) $EXTRA_INCLUDES
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

make -C "$BUILD_OBJDIR" -j"${FLATPAK_BUILDER_MAX_JOBS:-8}"

rm -f "$LIB"
ar rcs "$LIB" "$BUILD_OBJDIR"/*.o

install -Dm644 "$LIB" "$APP_PREFIX/lib/libgiac.a"
mkdir -p "$APP_PREFIX/include/giac"
cp "$SRCDIR_ABS"/*.h "$APP_PREFIX/include/giac/"

# The SDK toolchain (GCC 15) does not ship the libstdc++fs stub, but yutovo-solver
# v1.3.1 links -lstdc++fs.  Filesystem support lives in libstdc++ itself, so an
# empty archive is enough.  With BUILD_TESTS=OFF yutovo-solver also passes the raw
# name giac_imported to the linker; a linker script maps it to the real library.
ar rcs "$APP_PREFIX/lib/libstdc++fs.a"
printf 'INPUT ( libgiac.a )\n' > "$APP_PREFIX/lib/libgiac_imported.a"

echo "Giac 2.0.0 static library installed to: $APP_PREFIX/lib/libgiac.a"
