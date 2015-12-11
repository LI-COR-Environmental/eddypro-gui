#!/bin/sh

# shadow build in debug or release
# usage
# $ ./win-build-eddypro.sh [debug|release]

echo "### Running '$ $0 $@' in '$PWD'..."

if [ "$#" -ne 1 ]; then
  echo "Usage: $0 [debug|release]" >&2
  exit 1
fi
DEBUG_OR_RELEASE=$1

# set environment
echo "### Set up environment for Qt usage..."
QT_PATH="/c/qt/5.5.1/5.5/mingw492_32/bin"
MINGW_PATH="/c/qt/5.5.1/tools/mingw492_32/bin"
PATH=$QT_PATH:$PATH
PATH=$MINGW_PATH:$PATH

# build libs
win-build-quazip.sh $DEBUG_OR_RELEASE

# set build directory
app_name="eddypro"
qt_version="qt-5.5.1"
mingw_version="mingw-4.9.2"
platform="x86"
build_dir="build-$app_name-$qt_version-$mingw_version-$platform"

# set qmake project file
qmake_project="../../source/$app_name.pro"

# make shadow build
cd ../../../
mkdir -p "build/$build_dir"

cd "build/$build_dir"
echo "### Make shadow build in '$PWD'..."

echo "### Run 'qmake'..."
qmake -Wall $qmake_project

echo "### Run 'mingw32-make'..."
mingw32-make MAKE="mingw32-make -j4" $DEBUG_OR_RELEASE
