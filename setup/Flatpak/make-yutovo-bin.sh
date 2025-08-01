#!/bin/sh

rm -rf yutovo-bin-1.1.3
mkdir yutovo-bin-1.1.3
cd yutovo-bin-1.1.3
cp ../docker_build/build/src/yutovo_desktop .
cp -r ../../../../yutovo_server/library .
cp ../../../src/translations/*.qm .
cp ../yutovo.png ./com.yutovo.yutovo.png
cp ../yutovo.desktop ./com.yutovo.yutovo.desktop
cp ../com.yutovo.yutovo.metainfo.xml .
cp ../com.yutovo.yutovo.xml .
cp ../index.theme .
cp ../run.sh .

cd ..
tar -czvf yutovo-bin-1.1.3.tar.gz yutovo-bin-1.1.3/*