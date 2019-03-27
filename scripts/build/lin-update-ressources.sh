#!/bin/sh

UNZIP="unzip" # un-zip program

if [ ! -d fonts -o ! -d tra ] ; then
    echo "It seems script '`basename $0`' is not launched from project's root directory !"
    exit 1
fi

if [ "$1" = "clean" ] ; then
    CLEAN="true"
else
    CLEAN="false"
fi

$CLEAN && echo "Cleaning ressources files ... " || echo "Updating ressources files ... "

echo " > OpenSans fonts files"
if $CLEAN ; then
    rm -f fonts/OpenSans*.ttf fonts/Apache*.txt
else
    if [ -f fonts/OpenSans-Light.ttf ] ; then echo "    seems to be already unzipped" ; else cd fonts ; $UNZIP open-sans.zip ; cd - ; fi
fi

echo " > Translation files"
if $CLEAN ; then
    rm -f tra/*.qm
else
    if [ -z "`ls tra/*.qm`" ] ; then lrelease eddypro_lin.pro ; else echo "    seems to be already converted to binary (qm)" ; fi
fi

