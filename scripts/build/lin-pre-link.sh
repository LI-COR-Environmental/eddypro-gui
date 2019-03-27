#!/bin/sh


echo "Copying dynamic libraries in the debug or release build folder... "

if [ "$#" -eq 0 ]; then
    echo "Usage: $0 debug|release [project-root-dir]\n" 1>&2
    cat 1>&2 << END
Usage: $0 debug|release [system|local] [project-root-dir]

    debug or release : mandatory
        specifies which version of library is to be copied
        and which folder it copied to (before linking).
        This is ignored if system's library is used.

    system or local or auto : optional
        specifies if library to be used is system's or local one.
        If 'auto' (or anything else but 'system' or 'local'), use
        system's if any (according ldconfig), local's one otherwise.

    project-root-dir : optional
        specify project's directory, used for finding local library
        (libs/quazip-*)

In case of local library usage (previously made),
copy dynamic libraries in the debug or release build folder,
make appropriate links for linking and dynamic load,
and create bash launcher for appropriate use of library (if needed),
as no proper lib install is automatically done.

Note :
Debug version of quazip library may not be available on system.

END
    exit 1
fi

## arguments processing
if [ "$1" = "debug" ] ; then
    DEBUG_OR_RELEASE="debug"
    QUAZIP_LIB="libquazip_debug.so.1.0.0"
    BINFILE="eddypro_debug"
else
    DEBUG_OR_RELEASE="release"
    QUAZIP_LIB="libquazip.so.1.0.0"
    BINFILE="eddypro"
fi
# use local or system's lib ?
if [ -n "`ldconfig -p | grep libquazip`" ] ; then
    if [ "$2" = "local" ] ; then
        echo "warning : use of local lib quazip for building while existing in system : can lead to problems at execution." >&2
    else
        # i.e for system, auto, "" or anything
        echo "Make use of system quazip lib rather than build it => nothing to do"
        exit
    fi
elif [ "$2" = "system" ] ; then
    # no lib, but system required
    echo "system's library is required but none was detected on system (using ldconfig)" 1>&2
    exit 1
fi

if [ -n "$3" ] ; then
    ROOT_DIR="$3"
else
    # suppose ROOT_DIR is the same as git's repo name
    ROOT_DIR="../eddypro-gui"
fi


PWD=$(pwd)
echo "[pwd: $PWD]"

BUILD_DIR="./$DEBUG_OR_RELEASE"
echo "[BUILD_DIR: $BUILD_DIR]"

# search for any shadow building, if any
d=`ls -d $ROOT_DIR/libs/build-quazip*-$DEBUG_OR_RELEASE`
if [ -n "$d" ] ;then
    # found at least one dir
    if [ `echo "$d" | wc -l` -gt 1 ] ; then
        echo "    > too many building directories : can't choose one !" >&2 ; exit 1 ;
    fi
    QUAZIP_BUILD_DIR="`basename $d`"
else
# otherwise, use default in-place building
    QUAZIP_BUILD_DIR="quazip-0.7.1/quazip"
fi

echo "[QUAZIP_BUILD_DIR: $QUAZIP_BUILD_DIR]"

echo "Copy quazip in the app binary folder..."
cp "$ROOT_DIR/libs/$QUAZIP_BUILD_DIR/$QUAZIP_LIB" "$BUILD_DIR"
# link to .so file (for ld building)
cd $BUILD_DIR
qzlib=`basename $QUAZIP_LIB .1.0.0`
ln -s "$QUAZIP_LIB" "$qzlib"
# link to .so.1 file (for binary linking) at runtime
# not useful is properly installed
qzlib=`basename $QUAZIP_LIB .1.0.0`
ln -s "$QUAZIP_LIB" "$qzlib.1"
cd -

# add a launching wrapper, as a quick hack for installation lack
# not useful is properly installed
echo "Create sh launcher eddypro.sh"
cat > "$BUILD_DIR/eddypro.sh" << EOF
#!/bin/sh
BINDIR="\`dirname \$0\`"
export LD_LIBRARY_PATH="\$LD_LIBRARY_PATH:\$BINDIR"
\$BINDIR/${BINFILE}
EOF
chmod +x "$BUILD_DIR/eddypro.sh"

