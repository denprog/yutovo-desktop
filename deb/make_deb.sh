#!/bin/bash

rm yutovo-desktop_1.0.1-1_amd64/opt/yutovo/*
mkdir -p yutovo-desktop_1.0.1-1_amd64/opt/yutovo/

cp ../build/src/yutovo_desktop yutovo-desktop_1.0.1-1_amd64/opt/yutovo/
cp ../build/src/doc/first_page_ru.yut yutovo-desktop_1.0.1-1_amd64/opt/yutovo/
cp ../build/src/doc/first_page_en.yut yutovo-desktop_1.0.1-1_amd64/opt/yutovo/

cp ../../yutovo_editor/keys/yutovo_desktop_cert.pem yutovo-desktop_1.0.1-1_amd64/opt/yutovo/
cp ../../yutovo_editor/keys/yutovo_desktop_key.pem yutovo-desktop_1.0.1-1_amd64/opt/yutovo/
cp ../build/*.qm yutovo-desktop_1.0.1-1_amd64/opt/yutovo/

cp ../../yutovo_solver/build/src/yutovo_serviced yutovo-desktop_1.0.1-1_amd64/opt/yutovo/
cp ../../yutovo_solver/keys/yutovo_solver.crt yutovo-desktop_1.0.1-1_amd64/opt/yutovo/
cp ../../yutovo_solver/keys/yutovo_solver.key yutovo-desktop_1.0.1-1_amd64/opt/yutovo/
cp ../../yutovo_solver/src/service.ini yutovo-desktop_1.0.1-1_amd64/opt/yutovo/

cp yutovo.desktop yutovo-desktop_1.0.1-1_amd64/usr/share/applications/

dpkg-deb --build yutovo-desktop_1.0.1-1_amd64