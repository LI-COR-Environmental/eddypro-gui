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
        LIBS += -L$$OUT_PWD/../../libs/mac/build-quazip-0.7.6-qt-5.11.2-clang-10.0.0-x86_64 -lquazip_debug

        # linking
        QMAKE_PRE_LINK += $$_PRO_FILE_PWD_/scripts/build/mac-pre-link.sh debug
        QMAKE_POST_LINK += $$_PRO_FILE_PWD_/scripts/build/mac-post-link.sh debug
    }
    linux {
        # quazip
        LIBS += -L$$OUT_PWD/../../libs/centos/build-quazip-0.7.3-qt-5.10.1-centos-gcc-4.8.5-x86_64/quazip -lquazip_debug

        # linking
        QMAKE_PRE_LINK += $$_PRO_FILE_PWD_/scripts/build/lin-pre-link.sh debug
    }
} else {
    win32 {
        # quazip
        LIBS += -lquazip

        QMAKE_PRE_LINK += $$_PRO_FILE_PWD_/scripts/build/win-pre-link-release.cmd
    }
    macx {
        # quazip
        LIBS += -L$$OUT_PWD/../../libs/mac/build-quazip-0.7.6-qt-5.11.2-clang-10.0.0-x86_64 -lquazip

        # linking
        QMAKE_PRE_LINK += $$_PRO_FILE_PWD_/scripts/build/mac-pre-link.sh release
        QMAKE_POST_LINK += $$_PRO_FILE_PWD_/scripts/build/mac-post-link.sh release
    }
    linux {
        # quazip
        LIBS += -L$$OUT_PWD/../../libs/centos/build-quazip-0.7.3-qt-5.10.1-centos-gcc-4.8.5-x86_64/quazip -lquazip

        # linking
        QMAKE_PRE_LINK += $$_PRO_FILE_PWD_/scripts/build/lin-pre-link.sh release
    }
}
