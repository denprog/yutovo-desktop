#!/bin/bash

rm -rf yutovo-desktop_1.6.1-1_ubuntu24_amd64/*
mkdir -p yutovo-desktop_1.6.1-1_ubuntu24_amd64/opt/yutovo/
mkdir -p yutovo-desktop_1.6.1-1_ubuntu24_amd64/usr/share/applications/
mkdir -p yutovo-desktop_1.6.1-1_ubuntu24_amd64/usr/share/icons/hicolor/256x256/apps/
mkdir -p yutovo-desktop_1.6.1-1_ubuntu24_amd64/usr/share/mime/packages/
mkdir -p yutovo-desktop_1.6.1-1_ubuntu24_amd64/usr/share/icons/hicolor/256x256/mimetypes/
mkdir -p yutovo-desktop_1.6.1-1_ubuntu24_amd64/DEBIAN/

cp control yutovo-desktop_1.6.1-1_ubuntu24_amd64/DEBIAN/
cp postinst yutovo-desktop_1.6.1-1_ubuntu24_amd64/DEBIAN/
cp prerm yutovo-desktop_1.6.1-1_ubuntu24_amd64/DEBIAN/
cp ../../build/release/src/yutovo-desktop yutovo-desktop_1.6.1-1_ubuntu24_amd64/opt/yutovo/
cp -r ../../../yutovo-library/library yutovo-desktop_1.6.1-1_ubuntu24_amd64/opt/yutovo/
cp -r ../../../yutovo-library/pack.sh yutovo-desktop_1.6.1-1_ubuntu24_amd64/opt/yutovo/
cd yutovo-desktop_1.6.1-1_ubuntu24_amd64/opt/yutovo/
./pack.sh
rm ./pack.sh
cd -
chmod -R -w yutovo-desktop_1.6.1-1_ubuntu24_amd64/opt/yutovo/library
cp ../../build/release/src/*.qm yutovo-desktop_1.6.1-1_ubuntu24_amd64/opt/yutovo/
cp ../../src/images/mainicon.png yutovo-desktop_1.6.1-1_ubuntu24_amd64/usr/share/icons/hicolor/256x256/apps/yutovo.png
cp ../../src/images/mainicon.png yutovo-desktop_1.6.1-1_ubuntu24_amd64/usr/share/icons/hicolor/256x256/mimetypes/application-x-yut.png

chmod g-w yutovo-desktop_1.6.1-1_ubuntu24_amd64/opt/yutovo
chmod o-w yutovo-desktop_1.6.1-1_ubuntu24_amd64/opt/yutovo

cp yutovo.desktop yutovo-desktop_1.6.1-1_ubuntu24_amd64/usr/share/applications/
cp ../yut.xml yutovo-desktop_1.6.1-1_ubuntu24_amd64/usr/share/mime/packages/

dpkg-deb --build yutovo-desktop_1.6.1-1_ubuntu24_amd64