#!/bin/sh

cp ../../../src/translations/*.qm .
cp ../yutovo.png .
cp ../yutovo.desktop .
cp ../com.yutovo.yutovo.metainfo.xml .
cp ../yutovo.xml .
cp ../index.theme .
cp ../run.sh .

cd ..
tar -czvf yutovo-bin-1.2.1.tar.gz yutovo-bin-1.2.1/*