# define a shadowed build tree inside the build directory
#
# calling qmake from the build directory we can avoid to prepend it
#
# actual build directory:
# BUILD_DIR = $$_PRO_FILE_PWD_/../build

CONFIG(debug, debug|release) {
    DESTDIR = debug
    QMAKE_LIBDIR += debug # path of the dynamic libs to link
} else {
    DESTDIR += release
    QMAKE_LIBDIR += release # path of the dynamic libs to link
}

OBJECTS_DIR += .obj
MOC_DIR += .moc
RCC_DIR += .res
UI_DIR = .ui
