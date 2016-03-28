# platform dependent resource files

# win
win32 {
    RC_ICONS += $$_PRO_FILE_PWD_/win_files/app.ico
}

# mac
macx {
    ICON = $$_PRO_FILE_PWD_/mac_files/app.icns

    CONFIG(debug, debug|release) {
        QMAKE_INFO_PLIST = $$_PRO_FILE_PWD_/mac_files/plist/debug/Info.plist
    } else {
        QMAKE_INFO_PLIST = $$_PRO_FILE_PWD_/mac_files/plist/release/Info.plist
    }
}

# linux
linux {
    ICON = $$_PRO_FILE_PWD_/lin_files/app.png
}

# common
CONFIG(release, debug|release) {
    # makes rcc to compress at the best level of zlib compression
    QMAKE_RESOURCE_FLAGS += -threshold 0 -compress 9
}

#message(QMAKE_INFO_PLIST: $$QMAKE_INFO_PLIST)
#message(ICON: $$ICON)
#message(_PRO_FILE_PWD_: $$_PRO_FILE_PWD_)
