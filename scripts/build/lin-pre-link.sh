#!/bin/sh
echo "Copying dynamic libraries in the debug or release build folder... "

DEBUG_OR_RELEASE=$1

if [ "$DEBUG_OR_RELEASE" = "debug" ]; then
  QUAZIP_LIB="libquazip_debug.so.1.0.0"
else
  QUAZIP_LIB="libquazip.so.1.0.0"
fi

PWD=$(pwd)
echo "[pwd: $PWD]"

BUILD_DIR="./$DEBUG_OR_RELEASE"
echo "[BUILD_DIR: $BUILD_DIR]"

QUAZIP_BUILD_DIR="build-quazip-0.7.1-qt-5.7.0-centos-gcc-4.8.5-x86_64/quazip"
echo "[QUAZIP_BUILD_DIR: $QUAZIP_BUILD_DIR]"

echo "Copy quazip in the app binary folder..."
cp "../libs/$QUAZIP_BUILD_DIR/$QUAZIP_LIB" "$BUILD_DIR"
