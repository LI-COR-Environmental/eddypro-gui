#!/bin/sh

echo "Copy dynamic libs in the debug or release binary folder..."
DEBUG_OR_RELEASE=$1

if [ "$DEBUG_OR_RELEASE" == "debug" ]
then
echo 'quazip debug'
    cp -f -u "../libs/build-quazip-0.7.1-qt-5.5.1-mingw-4.9.2-x86/$DEBUG_OR_RELEASE/quazipd.dll" debug
else
echo 'quazip release'
    cp -f -u "../libs/build-quazip-0.7.1-qt-5.5.1-mingw-4.9.2-x86/$DEBUG_OR_RELEASE/quazip.dll" release
fi
