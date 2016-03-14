#!/bin/bash

# shadow build in debug or release
# usage
# $ ./win-build-g3log.sh [debug|release]

echo "### Running '$ $0 $@' in '$PWD'..."

if [ "$#" -ne 1 ]; then
  echo "Usage: $0 [debug|release]" >&2
  exit 1
fi
DEBUG_OR_RELEASE=$1

# set environment only if necessary
# (if called by win-build-eddypro.sh it is already partially inherited
MINGW_PATH="/c/qt/5.4.1/tools/mingw482_32/bin"
CMAKE_PATH="/c/program files (x86)/cmake/bin"
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
idempotent_path_prepend()
{
    PATH=${PATH//":$@"/} #delete any instances in the middle or at the end
    PATH=${PATH//"$@:"/} #delete any instances at the beginning
    export PATH="$@:$PATH" #prepend to beginning
}
idempotent_path_prepend $MINGW_PATH
idempotent_path_prepend $CMAKE_PATH
echo "PATH: " $PATH

CMAKE_CXX_COMPILER=$MINGW_PATH/g++.exe
CMAKE_MAKE_PROGRAM=$MINGW_PATH/mingw32-make.exe

# set build directory
lib_name="g3log"
mingw_version="mingw-4.9.1"
platform="x86"
build_dir="build-$lib_name-$mingw_version-$platform"

# set qmake project file
#cmake_project_path="../../../source/libs/g3log/g2log/"

# make shadow build
cd ../../../
mkdir -p "build/libs/$build_dir"

cd "build/libs/$build_dir"
echo "### Make shadow build in '$PWD'..."

echo "### Run 'cmake'..."
# debug build type is not supported in the library cmake config file and not working
cmake -DCMAKE_MAKE_PROGRAM=$CMAKE_MAKE_PROGRAM -DCMAKE_BUILD_TYPE=$DEBUG_OR_RELEASE -G "MinGW Makefiles" $cmake_project_path

# build only the library
echo "### Run 'mingw32-make'..."
mingw32-make MAKE="mingw32-make -j4" g3logger_shared
