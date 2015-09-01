#!/bin/sh
echo "Run install_name_tool in the debug or release binary folder... "

DEBUG_OR_RELEASE=$1

if [ "$DEBUG_OR_RELEASE" == "debug" ]; then
  EDDYPRO_APP=eddypro_debug.app/Contents/MacOs/eddypro_debug
  QUAZIP_LIB=libquazip_debug.1.0.0.dylib
else
  EDDYPRO_APP=eddypro.app/Contents/MacOs/eddypro
  QUAZIP_LIB=libquazip.1.0.0.dylib
fi

PWD=$(pwd)
echo "[pwd: $PWD]"

echo "Run 'install_name_tool -change' to change QuaZIP install name in EddyPro binary..."
install_name_tool -change "$QUAZIP_LIB" "@loader_path/../Frameworks/$QUAZIP_LIB" ./$DEBUG_OR_RELEASE/$EDDYPRO_APP
