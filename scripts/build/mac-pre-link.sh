#!/bin/sh
echo "Copying frameworks and dynamic libraries in the debug or release binary folder... "

DEBUG_OR_RELEASE=$1

if [ "$DEBUG_OR_RELEASE" == "debug" ]; then
  FRAMEWORKS_DIR="./$DEBUG_OR_RELEASE/eddypro_debug.app/Contents/Frameworks"
  QUAZIP_LIB=libquazip_debug.1.0.0.dylib
else
  FRAMEWORKS_DIR="./$DEBUG_OR_RELEASE/eddypro.app/Contents/Frameworks"
  QUAZIP_LIB=libquazip.1.0.0.dylib
fi

PWD=$(pwd)
echo "[pwd: $PWD]"

echo "[FRAMEWORKS_DIR: $FRAMEWORKS_DIR]"

QUAZIP_BUILD_DIR="build-quazip-0.7.6-qt-5.11.3-clang-10.0.0-x86_64"
echo "[QUAZIP_BUILD_DIR: $QUAZIP_BUILD_DIR]"

echo "Create framework dir in the app bundle..."
mkdir -p $FRAMEWORKS_DIR

echo "Run 'install_name_tool -id' to change inner QuaZIP identification name..."
install_name_tool -id "@loader_path/../Frameworks/$QUAZIP_LIB" ../../libs/mac/$QUAZIP_BUILD_DIR/$QUAZIP_LIB

echo "Copy quazip in the app bundle..."
cp ../../libs/mac/$QUAZIP_BUILD_DIR/$QUAZIP_LIB $FRAMEWORKS_DIR
