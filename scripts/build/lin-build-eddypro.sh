#!/bin/sh

# shadow build in debug or release
# usage
# $ scripts/build/lin-build-eddypro.sh [debug|release]

echo "### Running '$ $0 $@' in '$PWD'..."

if [ $# -eq 0 ]; then
    cat 1>&2 << END
Usage: $0 [debug|release] [quazip-library]

    debug or release :
        build according debug or release profile.

    quazip-library : optional
        can be 'quazip', 'quazip-qt4', 'quazip-qt5', 'auto' or nothing  
        - local : use (and build) quazip's local library
        - quazip : use quazip's system library, whatever it is
        - quazip-qt4 : use quazip's system library, qt4 compiled version
        - quazip-qt5 : use quazip's system library, qt5 compiled version
        - auto or nothing : use quazip's system library if any, local's one otherwise

This script will extract and convert needed ressources files (translations, fonts),
compile quazip lib if needed, create a shadow building directory,
 and build eddypro application (with needed library and a launching script if
local quazip is used.

Re-launching this script should not re-do things already done.
If you wan't to rebuild from scratch, just remove building shadow directory
(but this will keep project's extracted files, such as fonts, translations or quazip files)

About quazip :
    you can use quazip library included in this repository (local version)
    (and a compilation will then be done)
    or use the quazip library of you're system (we then need dynamic
    library and headers). There might be then a problem of version.
    For more details, see README.linux and ./scripts/build/lin-build-quazip.sh.

Note : 
Must be launched from project's root directory, i.e.
$ scripts/build/lin-build-eddypro.sh

END
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
case "$2" in
    local) CFG="quazip-local" ;;
    quazip|quazip-qt4) CFG="quazip" ;;
    quazip-qt5) CFG="quazip-qt5" ;;
    *)
        if [ -n "`ldconfig -p | grep libquazip`" ] ; then
            CFG="quazip"
        else
            CFG="quazip-local"
        fi
        ;;
esac

# build libs
if [ "$CFG" = "quazip-local" -o  $DEBUG_OR_RELEASE = "debug" ] ; then
    $SRC_DIR/scripts/build/lin-build-quazip.sh $DEBUG_OR_RELEASE auto
fi

# update ressource
$SRC_DIR/scripts/build/lin-update-ressources.sh $DEBUG_OR_RELEASE

# set build directory
app_name="eddypro"
qt_version="qt-`qmake -v | grep Qt | cut -d ' ' -f 4`"
distrib=`{ lsb_release -ds 2>/dev/null || cat /etc/*release 2>/dev/null | grep PRETTY_NAME  cut -d = -f 2 ; } | sed 's/ /_/g;s/"//g;s/\//_/g;'`
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
    if [ "$$DEBUG_OR_RELEASE" = "debug" ] ; then
        qmake -Wall $qmake_project -spec linux-g++ CONFIG+=debug CONFIG+=qml_debug CONFIG+=$CFG
    else
        qmake -Wall $qmake_project -spec linux-g++ CONFIG+=$CFG
    fi
fi

echo "### Run 'make'..."
make $DEBUG_OR_RELEASE


