#!/bin/sh

# shadow build in debug or release
# usage
# $ ./win-build-wwwidgets.sh [debug|release]

echo "### Running '$ $0 $@' in '$PWD'..."

if [ "$#" -ne 1 ]; then
  echo "Usage: $0 [debug|release]" >&2
  exit 1
fi
DEBUG_OR_RELEASE=$1

# set environment for qt usage only if necessary
# (if called by win-build-eddypro.sh it is already inherited
QT_PATH="/c/qt/5.3.2/5.3/mingw482_32/bin"
MINGW_PATH="/c/qt/5.3.2/tools/mingw482_32/bin"
prepend_path()
{
    SAVED_IFS="$IFS"
    IFS=:
    for dir in $@ ; do
        if ! $( echo "$PATH" | tr ":" "\n" | grep -qx "$dir" ) ; then
            PATH=$dir:$PATH
        fi
    done
    IFS="$SAVED_IFS"
    unset dir
}
prepend_path $QT_PATH
prepend_path $MINGW_PATH

# set build directory
lib_name="wwwidgets"
qt_version="qt-5.3.2"
mingw_version="mingw-4.8.2"
platform="x86"
build_dir="build-$lib_name-$qt_version-$mingw_version-$platform"

# set qmake project file
qmake_project="../../../source/libs/wwwidgets-qt-5.3.1/widgets/widgets.pro"

# make shadow build
cd ../../../
mkdir -p "build/libs/$build_dir"

cd "build/libs/$build_dir"
echo "### Make shadow build in '$PWD'..."

echo "### Run 'qmake'..."
qmake -Wall $qmake_project

echo "### Run 'mingw32-make'..."
mingw32-make MAKE="mingw32-make -j4" $DEBUG_OR_RELEASE
