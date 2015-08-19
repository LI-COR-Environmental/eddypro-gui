# define libraries to build and link
CONFIG(debug, debug|release) {
    win32 {
        # g3logger
        #LIBS += -llibg3logger_shared

        # quazip
        LIBS += -lquazipd

        QMAKE_PRE_LINK += $$_PRO_FILE_PWD_/scripts/build/win-pre-link-debug.cmd
    }
    macx {
        # g3logger
        #LIBS += -L$$OUT_PWD/../libs/build-g3log-clang-5.1.0-x86_64 -lg3logger_shared

        # quazip
        # working but each build delete the install name in the executable, so it requires run install_name_tool after each build
        LIBS += -L$$OUT_PWD/../libs/build-quazip-qt-5.4.2-clang-6.1.0-x86_64 -lquazip_debug
#        LIBS += -L$$OUT_PWD/../libs/build-quazip-qt-5.5.0-clang-6.1.0-x86_64 -lquazip_debug

        # linking
        QMAKE_PRE_LINK += $$_PRO_FILE_PWD_/scripts/build/mac-pre-link.sh debug
        QMAKE_POST_LINK += $$_PRO_FILE_PWD_/scripts/build/mac-post-link.sh debug
    }
} else {
    win32 {
        # g3logger
        #LIBS += -llibg3logger_shared

        # quazip
        LIBS += -lquazip

        QMAKE_PRE_LINK += $$_PRO_FILE_PWD_/scripts/build/win-pre-link-release.cmd
    }
    macx {
        # g3logger
        #LIBS += -L$$OUT_PWD/../libs/build-g3log-clang-5.1.0-x86_64 -lg3logger_shared

        # quazip
        LIBS += -L$$OUT_PWD/../libs/build-quazip-qt-5.4.1-clang-6.1.0-x86_64 -lquazip

        # linking
        QMAKE_PRE_LINK += $$_PRO_FILE_PWD_/scripts/build/mac-pre-link.sh release
        QMAKE_POST_LINK += $$_PRO_FILE_PWD_/scripts/build/mac-post-link.sh release
    }
}
