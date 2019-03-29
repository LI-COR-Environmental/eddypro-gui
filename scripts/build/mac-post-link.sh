#!/bin/sh
echo "[Running $0]"
echo "Run install_name_tool in the debug or release binary folder... "

DEBUG_OR_RELEASE=$1
BUNDLE_NAME="$2.app"

echo "[arg DEBUG_OR_RELEASE: $DEBUG_OR_RELEASE]"
echo "[arg BUNDLE_NAME: $BUNDLE_NAME]"

PWD=$(pwd)
echo "[pwd: $PWD]"
echo "[ls: $(ls) $DEBUG_OR_RELEASE]"
echo "./$DEBUG_OR_RELEASE/eddypro.app"

if [ "$DEBUG_OR_RELEASE" == "debug" ]; then
    # change bundle name
    #mv -v "./$DEBUG_OR_RELEASE/eddypro_debug.app" "./$DEBUG_OR_RELEASE/$BUNDLE_NAME" || exit 1

    EDDYPRO_BUNDLE_DIR="$BUNDLE_NAME/Contents/MacOs/eddypro_debug"
    QUAZIP_LIB=libquazip_debug.1.0.0.dylib
else
    # change bundle name
    mv -v "./$DEBUG_OR_RELEASE/eddypro.app" "./$DEBUG_OR_RELEASE/$BUNDLE_NAME" || exit 1

    EDDYPRO_BUNDLE_DIR="$BUNDLE_NAME/Contents/MacOs/eddypro"
    QUAZIP_LIB=libquazip.1.0.0.dylib
fi

echo "[EDDYPRO_BUNDLE_DIR: "$EDDYPRO_BUNDLE_DIR"]"

echo "Run 'install_name_tool -change' to change QuaZIP install name in EddyPro binary..."
install_name_tool -change "$QUAZIP_LIB" "@loader_path/../Frameworks/$QUAZIP_LIB" "./$DEBUG_OR_RELEASE/$EDDYPRO_BUNDLE_DIR" || exit 2
