# EddyPro qmake project file

TEMPLATE = app

# EddyPro version
VER_MAJ = 6
VER_MIN = 2
VER_PAT = 2
VERSION = $$sprintf("%1.%2.%3",$$VER_MAJ,$$VER_MIN,$$VER_PAT)

# Qt version and path
QT_VER = $$[QT_VERSION]
QT_PATH = $$[QT_INSTALL_PREFIX]

# Qt config
QT += core gui widgets network concurrent
CONFIG += warn_on
CONFIG += debug_and_release
CONFIG += c++11

# Build tree with shadow building approach
include(build_tree.pri)

# Include paths
include(includes.pri)

# Libs to build and link
include(libs.pri)

# Platform specific configurations
include(platform.pri)

# Resource files
include(resources.pri)

# Source code files
include(sources.pri)

# Extra targets for automated tests
include(tests.pri)

CONFIG(debug, debug|release) {
    TARGET = eddypro_debug
    CONFIG += console

    DEFINES += QT_DEBUG

    # to suppress qt and 3rdparty library warnings
    QMAKE_CXXFLAGS += -isystem "$$QT_PATH/include"
    QMAKE_CXXFLAGS += -isystem "$$_PRO_FILE_PWD_/libs/quazip-0.7.1/quazip"

    win32 {
        # mingw warnings
        QMAKE_CXXFLAGS_WARN_ON += -pedantic -Wall -Wextra -Wdeprecated -Wcast-qual -Wwrite-strings -fdiagnostics-show-option
        QMAKE_CXXFLAGS_WARN_ON += -O0 -fno-inline -Wunused-result

        # to suppress compiler library warnings
        QMAKE_CXXFLAGS += -isystem "$$_PRO_FILE_PWD_/../../../libs/c++/boost_1_61_0"
        QMAKE_CXXFLAGS += -isystem "$$QT_PATH/../Tools/mingw492_32"
    }
    macx {
        QMAKE_CXXFLAGS += --system-header-prefix=boost/
        QMAKE_CXXFLAGS += -isystem "$$QT_PATH/lib/QtCore.framework/Headers"
        QMAKE_CXXFLAGS += -isystem "$$QT_PATH/lib/QtCore.framework/Versions/5/Headers"
        QMAKE_CXXFLAGS += -isystem "$$QT_PATH/lib/QtGui.framework/Headers"
        QMAKE_CXXFLAGS += -isystem "$$QT_PATH/lib/QtGui.framework/Versions/5/Headers"
        QMAKE_CXXFLAGS += -isystem "$$QT_PATH/lib/QtWidgets.framework/Headers"
        QMAKE_CXXFLAGS += -isystem "$$QT_PATH/lib/QtWidgets.framework/Versions/5/Headers"
        QMAKE_CXXFLAGS += -isystem "$$QT_PATH/lib/QtConcurrent.framework/Headers"
        QMAKE_CXXFLAGS += -isystem "$$QT_PATH/lib/QtConcurrent.framework/Versions/5/Headers"
        QMAKE_CXXFLAGS += -isystem "$$QT_PATH/lib/QtNetwork.framework/Headers"
        QMAKE_CXXFLAGS += -isystem "$$QT_PATH/lib/QtNetwork.framework/Versions/5/Headers"

        QMAKE_CXXFLAGS_WARN_ON += -Weverything
        QMAKE_CXXFLAGS_WARN_ON += -Wno-c++98-compat -Wno-c++98-compat-pedantic
        QMAKE_CXXFLAGS_WARN_ON += -Wno-padded
        QMAKE_CXXFLAGS_WARN_ON += -Wno-covered-switch-default
        QMAKE_CXXFLAGS_WARN_ON += -Wno-used-but-marked-unused # see http://lists.qt-project.org/pipermail/development/2014-March/016181.html
        QMAKE_CXXFLAGS_WARN_ON += -Wno-global-constructors # static objects
        QMAKE_CXXFLAGS_WARN_ON += -Wno-exit-time-destructors # static objects
        QMAKE_CXXFLAGS_WARN_ON += -Wno-documentation -Wno-documentation-unknown-command -Wno-documentation-deprecated-sync

        QMAKE_PRE_LINK += && $$_PRO_FILE_PWD_/scripts/build/mac-update-translations.sh$$escape_expand(\\n\\t)
    }
    unix {
        QMAKE_CXXFLAGS += -isystem "$$_PRO_FILE_PWD_/../../../libs/c++/boost_1_61_0"

        #QMAKE_CXXFLAGS_WARN_ON += -Wno-c++98-compat -Wno-c++98-compat-pedantic
        #QMAKE_CXXFLAGS_WARN_ON += -Wno-padded
        #QMAKE_CXXFLAGS_WARN_ON += -Wno-covered-switch-default
        #QMAKE_CXXFLAGS_WARN_ON += -Wno-used-but-marked-unused # see http://lists.qt-project.org/pipermail/development/2014-March/016181.html
        #QMAKE_CXXFLAGS_WARN_ON += -Wno-global-constructors # static objects
        #QMAKE_CXXFLAGS_WARN_ON += -Wno-exit-time-destructors # static objects
        #QMAKE_CXXFLAGS_WARN_ON += -Wno-documentation -Wno-documentation-unknown-command -Wno-documentation-deprecated-sync
    }
} else {
    TARGET = eddypro

    DEFINES -= QT_DEBUG
    DEFINES += QT_NO_DEBUG
    DEFINES += QT_NO_DEBUG_OUTPUT
    DEFINES += QT_NO_WARNING_OUTPUT

    macx {
        QMAKE_PRE_LINK += && $$_PRO_FILE_PWD_/scripts/build/mac-update-translations.sh$$escape_expand(\\n\\t)

        # remove debug symbols
        QMAKE_POST_LINK += && strip -S $$OUT_PWD/release/eddypro.app/Contents/MacOS/eddypro
    }
}

DEFINES += QT_NO_CAST_FROM_ASCII
DEFINES += QT_NO_CAST_FROM_BYTEARRAY
DEFINES += QT_NO_CAST_TO_ASCII
DEFINES += QT_NO_URL_CAST_FROM_STRING
DEFINES += QT_USE_QSTRINGBUILDER
DEFINES += QT_DEPRECATED_WARNINGS

#message(Qt version: $$QT_VERSION)
#message(Qt is installed in $$QT_INSTALL_PREFIX)
#message(Qt resources can be found in the following locations:)
#message(Documentation: $$QT_INSTALL_DOCS)
#message(Header files: $$QT_INSTALL_HEADERS)
#message(Libraries: $$QT_INSTALL_LIBS)
#message(Binary files (executables): $$QT_INSTALL_BINS)
#message(Plugins: $$QT_INSTALL_PLUGINS)
#message(Data files: $$QT_INSTALL_DATA)
#message(Translation files: $$QT_INSTALL_TRANSLATIONS)
#message(Settings: $$QT_INSTALL_SETTINGS)
#message(Examples: $$QT_INSTALL_EXAMPLES)
#message(Demonstrations: $$QT_INSTALL_DEMOS)

#build_pass:message(QMAKE_CFLAGS: $$QMAKE_CFLAGS)
#build_pass:message(QMAKE_CXXFLAGS: $$QMAKE_CXXFLAGS)
#message(QMAKE_CXXFLAGS: $$QMAKE_CXXFLAGS)

# workaround for QTBUG-34424
TR_EXCLUDE +=	$$_PRO_FILE_PWD_/../../../libs/c++/boost_1_61_0/*
