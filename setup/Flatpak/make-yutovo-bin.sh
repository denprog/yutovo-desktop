#!/bin/sh

rm -rf yutovo-bin-1.2.1
mkdir yutovo-bin-1.2.1
cd yutovo-bin-1.2.1
cp ../docker_build/build/src/yutovo_desktop .
cp -r ../../../../yutovo_server/library .
cp ../../../src/translations/*.qm .
cp ../yutovo.png .
cp ../yutovo.desktop .
cp ../com.yutovo.yutovo.metainfo.xml .
cp ../yutovo.xml .
cp ../index.theme .
cp ../run.sh .

cd ..
tar -czvf yutovo-bin-1.2.1.tar.gz yutovo-bin-1.2.1/*