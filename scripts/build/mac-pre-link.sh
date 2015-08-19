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
#G3LOGGER_LIB=libg3logger_shared.dylib

PWD=$(pwd)
echo "[pwd: $PWD]"

echo "[FRAMEWORKS_DIR: $FRAMEWORKS_DIR]"

QUAZIP_BUILD_DIR="build-quazip-qt-5.4.2-clang-6.1.0-x86_64"
#QUAZIP_BUILD_DIR="build-quazip-qt-5.5.0-clang-6.1.0-x86_64"
echo "[QUAZIP_BUILD_DIR: $QUAZIP_BUILD_DIR]"

#G3LOGGER_BUILD_DIR="build-g3log-clang-6.0.0-x86_64"
#echo "[G3LOGGER_BUILD_DIR: $G3LOGGER_BUILD_DIR]"

echo "Create framework dir in the app bundle..."
mkdir -p $FRAMEWORKS_DIR

echo "Run 'install_name_tool -id' to change inner QuaZIP identification name..."
install_name_tool -id "@loader_path/../Frameworks/$QUAZIP_LIB" ../libs/$QUAZIP_BUILD_DIR/$QUAZIP_LIB

#echo "Run 'install_name_tool -id' to change inner g3logger identification name..."
#install_name_tool -id "@loader_path/../Frameworks/$G3LOGGER_LIB" ../libs/$G3LOGGER_BUILD_DIR/$G3LOGGER_LIB

echo "Copy quazip in the app bundle..."
cp ../libs/$QUAZIP_BUILD_DIR/$QUAZIP_LIB $FRAMEWORKS_DIR

#echo "Copy g3logger in the app bundle..."
#cp ../libs/$G3LOGGER_BUILD_DIR/$G3LOGGER_LIB $FRAMEWORKS_DIR
