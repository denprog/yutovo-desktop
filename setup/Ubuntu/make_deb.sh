#!/bin/bash
set -e

if [ $# -ne 1 ]; then
    echo "Usage: $0 <version>   (e.g. 1.7.1)" >&2
    exit 1
fi

VERSION="$1"
if ! [[ "$VERSION" =~ ^[0-9]+(\.[0-9]+)*$ ]]; then
    echo "Invalid version: $VERSION" >&2
    exit 1
fi

PKG_DIR="yutovo-desktop_${VERSION}-1_ubuntu24_amd64"
BUILD_DIR="$(cd ../../build/release && pwd)"

if [ -d "$PKG_DIR" ]; then
    chmod -R u+w "$PKG_DIR"   # chmod -R -w on library below makes the tree undeletable without this
    rm -rf "$PKG_DIR"
fi
mkdir -p "$PKG_DIR/opt/yutovo/"
mkdir -p "$PKG_DIR/usr/share/applications/"
mkdir -p "$PKG_DIR/usr/share/icons/hicolor/256x256/apps/"
mkdir -p "$PKG_DIR/usr/share/mime/packages/"
mkdir -p "$PKG_DIR/usr/share/icons/hicolor/256x256/mimetypes/"
mkdir -p "$PKG_DIR/DEBIAN/"

rm -rf "$BUILD_DIR/library"
(cd ../../../yutovo-library && ./make_library.sh "$BUILD_DIR" ZIP)

sed "s/^Version: .*/Version: ${VERSION}/" control > "$PKG_DIR/DEBIAN/control"
cp postinst "$PKG_DIR/DEBIAN/"
cp prerm "$PKG_DIR/DEBIAN/"
cp "$BUILD_DIR/src/yutovo-desktop" "$PKG_DIR/opt/yutovo/"
cp "$BUILD_DIR/src/yutovo-solver-calculator-worker" "$PKG_DIR/opt/yutovo/"
cp -r "$BUILD_DIR/library" "$PKG_DIR/opt/yutovo/"
chmod -R -w "$PKG_DIR/opt/yutovo/library"
cp "$BUILD_DIR"/src/*.qm "$PKG_DIR/opt/yutovo/"
cp ../../src/images/mainicon.png "$PKG_DIR/usr/share/icons/hicolor/256x256/apps/yutovo.png"
cp ../../src/images/mainicon.png "$PKG_DIR/usr/share/icons/hicolor/256x256/mimetypes/application-x-yut.png"

chmod g-w "$PKG_DIR/opt/yutovo"
chmod o-w "$PKG_DIR/opt/yutovo"

cp yutovo.desktop "$PKG_DIR/usr/share/applications/"
cp ../yut.xml "$PKG_DIR/usr/share/mime/packages/"

dpkg-deb --build "$PKG_DIR"
