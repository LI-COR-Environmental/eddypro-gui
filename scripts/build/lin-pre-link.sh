#!/bin/sh
echo "Copying dynamic libraries in the debug or release build folder... "

DEBUG_OR_RELEASE=$1
if [ -n "$2" ] ; then
    ROOT_DIR="$2"
else
    ROOT_DIR="../eddypro-gui"
fi

if [ "$DEBUG_OR_RELEASE" = "debug" ]; then
  QUAZIP_LIB="libquazip_debug.so.1.0.0"
else
  QUAZIP_LIB="libquazip.so.1.0.0"
fi

PWD=$(pwd)
echo "[pwd: $PWD]"

BUILD_DIR="./$DEBUG_OR_RELEASE"
echo "[BUILD_DIR: $BUILD_DIR]"

# search for any shadow building, if any
d=`ls -d $ROOT_DIR/libs/build-${QZ}*`
if [ -n "$d" ] ;then
    # found at least one dir
    if [ `echo "$d" | wc -l` -gt 1 ] ; then
        echo "    > too many building directories : can't choose one !" ; exit 1 ;
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
# link to .so.1 file (for binary linking)
qzlib=`basename $QUAZIP_LIB .1.0.0`
ln -s "$QUAZIP_LIB" "$qzlib.1"
cd -
# add a launching wrapper, as a quick hack for installation lack
cat > "$BUILD_DIR/eddypro.sh" << EOF
#!/bin/sh
BINDIR="\`dirname \$0\`"
export LD_LIBRARY_PATH="\$LD_LIBRARY_PATH:\$BINDIR"
\$BINDIR/eddypro_${DEBUG_OR_RELEASE}
EOF
chmod +x "$BUILD_DIR/eddypro.sh"

