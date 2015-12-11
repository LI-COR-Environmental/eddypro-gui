# EddyPro qmake project file

TEMPLATE = app

# EddyPro version, not used
VERSION = 6.1.0

# Qt version and path
QT_VER = $$[QT_VERSION]
QT_PATH = "$$[QT_INSTALL_LIBS]/.."

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
    CONFIG += console precompile_header

    DEFINES += QT_DEBUG

    # to suppress qt and 3rdparty library warnings
    QMAKE_CXXFLAGS += -isystem"$QT_PATH/include"
    QMAKE_CXXFLAGS += -isystem"$$_PRO_FILE_PWD_/../../../../libs/c++/boost_1_55_0"

    win32 {
        # mingw warnings
        QMAKE_CXXFLAGS_WARN_ON += -pedantic -Wall -Wextra -Wcast-qual -Wwrite-strings -fdiagnostics-show-option
        QMAKE_CXXFLAGS_WARN_ON += -O0 -fno-inline

        # to suppress compiler library warnings
        QMAKE_CXXFLAGS += -isystem"$QT_PATH/../Tools/mingw492_32"
    }
} else {
    TARGET = eddypro

    DEFINES += QT_NO_DEBUG
    DEFINES += QT_NO_DEBUG_OUTPUT
    DEFINES += QT_NO_WARNING_OUTPUT

    macx {
        QMAKE_PRE_LINK += $$_PRO_FILE_PWD_/scripts/build/mac-update-translations.sh$$escape_expand(\\n\\t)

        # remove debug symbols
        QMAKE_POST_LINK = strip $$OUT_PWD/release/eddypro.app/Contents/MacOS/eddypro
    }
}

DEFINES += QT_NO_CAST_FROM_ASCII
DEFINES += QT_NO_CAST_FROM_BYTEARRAY
DEFINES += QT_NO_CAST_TO_ASCII
DEFINES += QT_NO_URL_CAST_FROM_STRING
DEFINES += QT_USE_QSTRINGBUILDER
