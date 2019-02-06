#!/bin/sh

# shadow build in debug or release
# usage
# $ scripts/build/lin-build-eddypro.sh [debug|release]

echo "### Running '$ $0 $@' in '$PWD'..."

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 [debug|release]\n\tMust be launched from project's root directory" 1>&2
    exit 1
fi
if [ "$1" = "debug" ] ; then
    DEBUG_OR_RELEASE=$1
else
    DEBUG_OR_RELEASE="release"
fi
if [ ! -f eddypro_lin.pro ] ; then
  echo "seems not to be launched from project's root directory" >&2
  exit 1
fi
SRC_DIR="$PWD"

# build libs
$SRC_DIR/scripts/build/lin-build-quazip.sh $DEBUG_OR_RELEASE auto

# update ressource
$SRC_DIR/scripts/build/lin-update-ressources.sh $DEBUG_OR_RELEASE

# set build directory
app_name="eddypro"
qt_version="qt-`qmake -v | grep Qt | cut -d ' ' -f 4`"
distrib=`{ lsb_release -ds 2>/dev/null || cat /etc/*release 2>/dev/null | grep PRETTY_NAME  cut -d = -f 2 ; } | sed 's/ /_/g;s/"//g;'`
[ -z "$distrib" ] && distrib="linux"
platform=`uname -m`
build_dir="build-$app_name-$DEBUG_OR_RELEASE+$qt_version+$distrib+$platform"

# set qmake project file
qmake_project="$SRC_DIR/eddypro_lin.pro"

# make shadow build
#SHADOW_DIR="../build/$build_dir"
SHADOW_DIR="../$build_dir"
if [ ! -d "$SHADOW_DIR" ] ; then
    echo "### Use shadow build $SHADOW_DIR"
    mkdir -p "$SHADOW_DIR"
fi
echo "### Make shadow build in '$PWD'..."
cd "$SHADOW_DIR"
if [ ! -f Makefile ] ; then
    echo "### Run 'qmake'..."
    qmake -Wall $qmake_project
fi

echo "### Run 'make'..."
make $DEBUG_OR_RELEASE


