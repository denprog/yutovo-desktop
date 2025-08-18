#!/bin/sh

rm -rf yutovo-bin-1.1.3
mkdir yutovo-bin-1.1.3
cd yutovo-bin-1.1.3
cp ../docker_build/build/src/yutovo-desktop .
cp -r ../../../../yutovo-server/library .
cp ../../../src/translations/*.qm .
cp ../yutovo.png .
cp ../yutovo.desktop .
cp ../com.yutovo.yutovo.metainfo.xml .
cp ../yutovo.xml .
cp ../index.theme .
cp ../run.sh .

cd ..
tar -czvf yutovo-bin-1.1.3.tar.gz yutovo-bin-1.1.3/*