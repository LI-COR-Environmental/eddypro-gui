#!/bin/sh
echo "[Running $0]"
echo "Run install_name_tool in the debug or release binary folder... "

DEBUG_OR_RELEASE=$1

echo "[arg DEBUG_OR_RELEASE: $DEBUG_OR_RELEASE]"

PWD=$(pwd)
echo "[pwd: $PWD]"
echo "[ls: $(ls) $DEBUG_OR_RELEASE]"
echo "./$DEBUG_OR_RELEASE/eddypro.app"

if [ "$DEBUG_OR_RELEASE" == "debug" ]; then
    EDDYPRO_APP=eddypro_debug.app/Contents/MacOs/eddypro_debug
    QUAZIP_LIB=libquazip_debug.1.0.0.dylib
else
    EDDYPRO_APP=eddypro.app/Contents/MacOs/eddypro
    QUAZIP_LIB=libquazip.1.0.0.dylib
fi

echo "[EDDYPRO_BUNDLE_DIR: "$EDDYPRO_BUNDLE_DIR"]"

echo "Run 'install_name_tool -change' to change QuaZIP install name in EddyPro binary..."
install_name_tool -change "$QUAZIP_LIB" "@loader_path/../Frameworks/$QUAZIP_LIB" ./$DEBUG_OR_RELEASE/$EDDYPRO_APP || exit 1
