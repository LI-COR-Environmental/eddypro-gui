# platform specific configurations

macx {
    CONFIG += x86
    CONFIG += app_bundle

    # 10.7 = lion
    # 10.8 = mountain lion
    # 10.9 = mavericks (current minimum supported)
    # 10.10 = yosemite
    # 10.11 = el capitan
    # 10.12 = sierra
#    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.9
#    QMAKE_MAC_SDK = macosx
#    QMAKE_MAC_SDK.macosx.version = 10.12
#    QMAKE_MAC_SDK = macosx10.12

    QMAKE_CFLAGS += -gdwarf-2
    QMAKE_CXXFLAGS += -gdwarf-2

    RESOURCES += $$_PRO_FILE_PWD_/eddypro_mac.qrc
}

win32 {
    RESOURCES += $$_PRO_FILE_PWD_/eddypro_win.qrc
}

linux {
    RESOURCES += $$_PRO_FILE_PWD_/eddypro_lin.qrc
}

# to copy files in the bundle
#macx {
#    jpegtran.path = Contents/MacOS
#    jpegtran.files = /Users/maloney/dev/bin/jpegtran
#    QMAKE_BUNDLE_DATA += jpegtran
#}

# or in the bin folder
#win32 {
#   EXTRA_BINFILES_WIN = ..\\bin\\jpegtran.exe
#   for( FILE, EXTRA_BINFILES_WIN ) {
#      QMAKE_PRE_LINK += $$quote(cmd /c copy /y $${FILE} $${DEBUG_OR_RELEASE}$$escape_expand(\\n\\t))
#    }
#}
