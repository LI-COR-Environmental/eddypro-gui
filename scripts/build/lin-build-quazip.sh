#!/bin/sh

QZ="quazip-0.7.1"

# shadow build in debug or release
# usage
# $ scripts/build/lin-build-eddypro.sh [debug|release]

echo "### Running '$ $0 $@' in '$PWD'..."

if [ "$#" -eq 0 ]; then
    cat 1>&2 << END
Usage: $0 debug|release [dir-or-auto]
    
    debug or release : mandatory
        build in debug or release profile
    dir-or-auto : optional
        specify shadow directory where build will be made.
        If 'auto', automatic creation of shadow directory is done.
        If not used, build will be made "in-place", i.e. in quazip
        ($QZ) directory.

Quazip libray used is the one ($QZ) in libs directory.
Patch is applied, shadow directory created (if needed),
qmake applied (Makefile generation), then make.

Note:
If compilation directory is not 'auto' or 'in-place', i.e. custom
defined, pre-link script won't find appropriate path, unless specified as option.
In such a case, main building script can't be used.

Note : 
Must be launched from project's root directory (for paths finding).

Dependencies :
    quazip depends on zlib (library and headers)
END
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
        # hard to simply know whether patch has already been applied !
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
    # if Makefile is already existing, guess that qmake was already launched before,
    # hence don't do it again
    echo "  > Launch qmake (in-place building)"
    if [ "$$DEBUG_OR_RELEASE" = "debug" ] ; then
        qmake -Wall $qmake_project -spec linux-g++ CONFIG+=debug CONFIG+=qml_debug
    else
        qmake -Wall $qmake_project -spec linux-g++
    fi
fi

echo "  > Launch make for $DEBUG_OR_RELEASE"
make $DEBUG_OR_RELEASE

