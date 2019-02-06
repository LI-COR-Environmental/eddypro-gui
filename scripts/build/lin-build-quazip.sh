#!/bin/sh


# shadow build in debug or release
# usage
# $ scripts/build/lin-build-eddypro.sh [debug|release]

echo "### Running '$ $0 $@' in '$PWD'..."

if [ "$#" -eq 0 ]; then
    echo "Usage: $0 debug|release [dir-or-auto]\n\tMust be launched from project's root directory" 1>&2
    exit 1
fi
if [ "$1" = "debug" ] ; then
    DEBUG_OR_RELEASE="debug"
else
    DEBUG_OR_RELEASE="release"
fi
if [ ! -d libs ] ; then
  echo "seems not to be launched from project's root directory" >&2
  exit 1
fi


QZ="quazip-0.7.1"
# full path is (from root) :
# libs/<QZ>/quazip
cd libs
if [ ! -d $QZ ] ; then
    echo "  > Extracting archive"
    if [ -f $QZ.tar.gz ] ; then tar zxvf $QZ.tar.gz ; else echo "    Missing archive file '$QZ.tar.gz' !" >&2 ; exit 1 ; fi
fi
cd $QZ || { echo "    Can't go to $QZ directory" >&2 ; exit 2 ; }
cd quazip
SRC_DIR="$PWD"
# apply patch
if [ -f ../../$QZ.patch ] ; then
    echo "  > Apply patch"
    if [ -n "`grep '\-lz' quazip.pro`" ] ; then # this test depends on patch content
        # hard to know simply know whether patch has already been applied !
        # but applying it twice is no good
        echo  "    seems that patch has already been applied"
    else
        patch -b < ../../$QZ.patch
    fi
fi

build_dir="build-$QZ-$DEBUG_OR_RELEASE"
qmake_project="$SRC_DIR/quazip.pro"

# make shadow build
# if $2 is defined : can be an arbitrary building shadow directory
# or 
if [ -n "$2" ] ; then
    if [ "$2" = "auto" ] ; then
        SHADOW_DIR="../../$build_dir"
    else
        cd ../..
        SHADOW_DIR="$2"
    fi
    mkdir -p "$SHADOW_DIR"
    cd "$SHADOW_DIR"
fi

echo "### Make shadow build in '$PWD'..."

if [ ! -f Makefile ] ; then
    echo "  > Launch qmake (in-place building)"
    qmake -Wall $qmake_project
fi

echo "  > Launch make for $DEBUG_OR_RELEASE"
make $DEBUG_OR_RELEASE

