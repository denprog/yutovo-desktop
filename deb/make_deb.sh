#!/bin/bash

rm -rf yutovo-desktop_1.0.1-1_amd64/*
mkdir -p yutovo-desktop_1.0.1-1_amd64/opt/yutovo/
mkdir -p yutovo-desktop_1.0.1-1_amd64/usr/share/applications/
mkdir -p yutovo-desktop_1.0.1-1_amd64/usr/share/icons/hicolor/256x256/apps/
mkdir -p yutovo-desktop_1.0.1-1_amd64/DEBIAN/

cp control yutovo-desktop_1.0.1-1_amd64/DEBIAN/
cp ../build/RelWithDebugInfo/src/yutovo_desktop yutovo-desktop_1.0.1-1_amd64/opt/yutovo/
cp -r ../../yutovo_server/library yutovo-desktop_1.0.1-1_amd64/opt/yutovo/
cp ../build/*.qm yutovo-desktop_1.0.1-1_amd64/opt/yutovo/
cp ../src/images/mainicon.png yutovo-desktop_1.0.1-1_amd64/usr/share/icons/hicolor/256x256/apps/yutovo.png

chmod g-w yutovo-desktop_1.0.1-1_amd64/opt/yutovo/yutovo_desktop
chmod o-w yutovo-desktop_1.0.1-1_amd64/opt/yutovo/yutovo_desktop

cp yutovo.desktop yutovo-desktop_1.0.1-1_amd64/usr/share/applications/

dpkg-deb --build yutovo-desktop_1.0.1-1_amd64