#!/bin/sh


if [ "$1" = "debug" ]; then
    DEBUG_OR_RELEASE=$1
else
    DEBUG_OR_RELEASE="release"
fi

QZ="quazip-0.7.1"
# full path is (from root) :
# libs/<QZ>/quazip
cd libs
if [ ! -d $QZ ] ; then
    echo "  > Extracting archive"
    if [ -f $QZ.tar.gz ] ; then tar zxvf $QZ.tar.gz ; else echo "    Missing archive file '$QZ.tar.gz' !" ; exit 1 ; fi
fi
cd $QZ || { echo "    Can't go to $QZ directory" ; exit 2 ; }
cd quazip
# apply patch
if [ -f ../../$QZ.patch ] ; then
    echo "  > Apply patch"
    if [ -n "`grep '\-lz' quazip.pro`" ] ; then # this test depends on patch content
        # hard to know simply know whether patch has already benn applied !
        # but applying it twice is no good
        echo  "    seems that patch has already been apllied"
    else
        patch -b < ../../$QZ.patch
    fi
fi

if [ ! -f Makefile ] ; then
    echo "  > Launch qmake (in-place building)"
    qmake
fi

echo "  > Launch make for $DEBUG_OR_RELEASE"
make $DEBUG_OR_RELEASE

