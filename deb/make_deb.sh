#!/bin/bash

rm yutovo-desktop_1.0.1-1_amd64/opt/yutovo/*
mkdir -p yutovo-desktop_1.0.1-1_amd64/opt/yutovo/
cp ../build/src/yutovo_desktop yutovo-desktop_1.0.1-1_amd64/opt/yutovo/
cp ../../yutovo_editor/keys/yutovo_desktop_cert.pem yutovo-desktop_1.0.1-1_amd64/opt/yutovo/
cp ../../yutovo_editor/keys/yutovo_desktop_key.pem yutovo-desktop_1.0.1-1_amd64/opt/yutovo/
cp ../build/*.qm yutovo-desktop_1.0.1-1_amd64/opt/yutovo/

cp ../../yutovo_service/build/src/yutovo_serviced yutovo-desktop_1.0.1-1_amd64/opt/yutovo/
cp ../../yutovo_service/keys/yutovo_service.crt yutovo-desktop_1.0.1-1_amd64/opt/yutovo/
cp ../../yutovo_service/keys/yutovo_service.key yutovo-desktop_1.0.1-1_amd64/opt/yutovo/
cp ../../yutovo_service/src/service.ini yutovo-desktop_1.0.1-1_amd64/opt/yutovo/

dpkg-deb --build yutovo-desktop_1.0.1-1_amd64