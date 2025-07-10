#!/bin/sh

rm -rf yutovo-bin-1.1.1
mkdir yutovo-bin-1.1.1
cd yutovo-bin-1.1.1
cp ../docker_build/build/src/yutovo_desktop .
cp -r ../../../../yutovo_server/library .
cp ../../../src/translations/*.qm .
cp ../yutovo.png ./com.yutovo.yutovo.png
cp ../yutovo.desktop ./com.yutovo.yutovo.desktop
cp ../com.yutovo.yutovo.metainfo.xml .
cp ../run.sh .

cd ..
tar -czvf yutovo-bin-1.1.1.tar.gz yutovo-bin-1.1.1/*