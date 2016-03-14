# define libraries to build and link
CONFIG(debug, debug|release) {
    win32 {
        # quazip
        LIBS += -lquazipd

        QMAKE_PRE_LINK += $$_PRO_FILE_PWD_/scripts/build/win-pre-link-debug.cmd
    }
    macx {
        # quazip
        # working but each build delete the install name in the executable, so it requires run install_name_tool after each build
        LIBS += -L$$OUT_PWD/../libs/build-quazip-0.7.1-qt-5.6.0-clang-7.0.2-x86_64 -lquazip_debug

        # linking
        QMAKE_PRE_LINK += $$_PRO_FILE_PWD_/scripts/build/mac-pre-link.sh debug
        QMAKE_POST_LINK += $$_PRO_FILE_PWD_/scripts/build/mac-post-link.sh debug
    }
    linux {
        # quazip
        LIBS += -L$$OUT_PWD/../libs/build-quazip-0.7.1-qt-5.5.1-gcc-5.2.0-x86/quazip -lquazip_debug

        # linking
        QMAKE_PRE_LINK += $$_PRO_FILE_PWD_/scripts/build/lin-pre-link.sh debug
#        QMAKE_POST_LINK += $$_PRO_FILE_PWD_/scripts/build/mac-post-link.sh debug
    }
} else {
    win32 {
        # quazip
        LIBS += -lquazip

        QMAKE_PRE_LINK += $$_PRO_FILE_PWD_/scripts/build/win-pre-link-release.cmd
    }
    macx {
        # quazip
        LIBS += -L$$OUT_PWD/../libs/build-quazip-0.7.1-qt-5.6.0-clang-7.0.2-x86_64 -lquazip

        # linking
        QMAKE_PRE_LINK += $$_PRO_FILE_PWD_/scripts/build/mac-pre-link.sh release
        QMAKE_POST_LINK += $$_PRO_FILE_PWD_/scripts/build/mac-post-link.sh release
    }
    linux {
        # quazip
        LIBS += -L$$OUT_PWD/../libs/build-quazip-0.7.1-qt-5.5.1-gcc-5.2.0-x86_64 -lquazip

        # linking
        QMAKE_PRE_LINK += $$_PRO_FILE_PWD_/scripts/build/lin-pre-link.sh release
#        QMAKE_POST_LINK += $$_PRO_FILE_PWD_/scripts/build/mac-post-link.sh release
    }
}
