# EddyPro
TEMPLATE = app
VERSION = 5.2.1

# config
QT += core gui widgets network xml help concurrent

CONFIG += warn_on
CONFIG += debug_and_release
CONFIG += c++11

CONFIG(debug, debug|release) {
    TARGET = eddypro_debug
    CONFIG += console

    QMAKE_LIBDIR += ./debug
    DESTDIR += ./debug

    DEFINES += QT_DEBUG

    win32 {
        LIBS += -L../build/libs/build-wwwidgets-qt-5.3.2-mingw-4.8.2-x86/debug -lwwwidgets5d
        LIBS += -L../build/libs/build-quazip-qt-5.3.2-mingw-4.8.2-x86/debug -lquazipd
        QMAKE_CXXFLAGS_WARN_ON += -pedantic -Wall -Wextra -Wcast-qual -Wwrite-strings -fdiagnostics-show-option
        QMAKE_CXXFLAGS_WARN_ON += -O0 -fno-inline

        # to suppress libraries warnings
        QMAKE_CXXFLAGS += -isystem"C:/Qt/5.3.2/5.3/mingw482_32"
        QMAKE_CXXFLAGS += -isystem"C:/Qt/5.3.2/Tools/mingw482_32"
        QMAKE_CXXFLAGS += -isystem"./libs/wwwidgets-qt-5.3.2/widgets"
        QMAKE_CXXFLAGS += -isystem"c:/devel/libs/c++/boost_1_55_0"

        QMAKE_PRE_LINK += $$_PRO_FILE_PWD_/scripts/build/win-pre-link-debug.bat
    }
    macx {
        QMAKE_LFLAGS_SONAME = -Wl,-install_name,@executable_path/../Frameworks/wwwidgets5.framework/Versions/1/wwwidgets5
        # requires an absolute path
        QMAKE_LFLAGS += -v -F/Users/antonioforgione1/devel/ep-5/ep5-github/build/libs/build-widgets-Desktop_Qt_5_3_2_clang_64bit
        LIBS += -framework wwwidgets5

        # working but each build delete the install name in the executable, so it requires run install_name_tool after each build
        LIBS += -L/Users/antonioforgione1/devel/ep-5/ep5-github/build/libs/build-quazip-qt-5.3.2-mingw-4.8.2-x86 -lquazip_debug

        QMAKE_PRE_LINK += $$_PRO_FILE_PWD_/scripts/build/mac-pre-link-debug.sh
        QMAKE_POST_LINK += $$_PRO_FILE_PWD_/scripts/build/mac-post-link-debug.sh

        # to suppress libraries warnings
        QMAKE_CXXFLAGS += -isystem"/Users/antonioforgione1/Qt/5.3.2/5.3/clang_64/include"
        QMAKE_CXXFLAGS += -isystem"./libs/wwwidgets-qt-5.3.2/widgets"
        QMAKE_CXXFLAGS += -isystem"/Users/antonioforgione1/devel/libs/c++/boost_1_55_0"
    }
} else {
    TARGET = eddypro

    QMAKE_LIBDIR += ./release
    DESTDIR += ./release

    DEFINES += QT_NO_DEBUG
    DEFINES += QT_NO_DEBUG_OUTPUT
    DEFINES += QT_NO_WARNING_OUTPUT

    # makes rcc to compress always and at the best level of zlib compression
    QMAKE_RESOURCE_FLAGS += -threshold 0 -compress 9

    win32 {
        LIBS += -L../build/libs/build-wwwidgets-qt-5.3.2-mingw-4.8.2-x86/release -lwwwidgets5
        LIBS += -L../build/libs/build-quazip-qt-5.3.2-mingw-4.8.2-x86/quazip/release -lquazip
        QMAKE_PRE_LINK += $$_PRO_FILE_PWD_/scripts/build/win-pre-link-release.bat
    }

    macx {
        QMAKE_LFLAGS_SONAME  = -Wl,-install_name,@executable_path/../Frameworks/wwwidgets5.framework/Versions/1/wwwidgets5
        QMAKE_LFLAGS += -F/Users/antonioforgione1/devel/ep-5/ep5/libs/wwwidgets-patched/widgets
        LIBS += -framework wwwidgets5

        QMAKE_LFLAGS += -F/Users/antonioforgione1/devel/ep-5/ep5-github/build/libs/build-quazip-qt-5.3.2-mingw-4.8.2-x86
        LIBS += -L../build/libs/build-quazip-qt-5.3.2-mingw-4.8.2-x86 -lquazip.dylib

        LIBS += -dead-strip

        QMAKE_PRE_LINK += $$_PRO_FILE_PWD_/scripts/build/mac-update-translations.sh$$escape_expand(\\n\\t)
        QMAKE_POST_LINK=strip eddypro.app/Contents/MacOS/eddypro
    }
}

macx {
    CONFIG += x86
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7
    QMAKE_MAC_SDK = macosx

    QMAKE_CFLAGS += -gdwarf-2
    QMAKE_CXXFLAGS += -gdwarf-2
}

INCLUDEPATH += .
INCLUDEPATH += ./libs/wwwidgets-patched/widgets
INCLUDEPATH += ./libs/quazip-trunk-2013-10-15/quazip
win32:INCLUDEPATH += c:/devel/libs/c++/boost_1_55_0
macx:INCLUDEPATH += /Users/antonioforgione1/devel/libs/c++/boost_1_55_0
OBJECTS_DIR += ./obj
MOC_DIR += ./moc
RCC_DIR += ./res

# Input
HEADERS += \
    src/globalsettings.h \
    src/widget_utils.h \
    src/aboutdialog.h \
    src/alia.h \
    src/anem_delegate.h \
    src/anem_desc.h \
    src/anem_model.h \
    src/anem_tableview.h \
    src/anem_view.h \
    src/binarysettingsdialog.h \
    src/biomitem.h \
    src/biommetadatareader.h \
    src/bminidefs.h \
    src/clicklabel.h \
    src/configstate.h \
    src/customcombomodel.h \
    src/customheader.h \
    src/customsplashscreen.h \
    src/dbghelper.h \
    src/defs.h \
    src/dlinidefs.h \
    src/dlinidialog.h \
    src/dlinstrtab.h \
    src/dlproject.h \
    src/dlprojectstate.h \
    src/dlrawfiledesctab.h \
    src/dlsitetab.h \
    src/docchooser.h \
    src/ecinidefs.h \
    src/ecproject.h \
    src/ecprojectstate.h \
    src/faderwidget.h \
    src/fileutils.h \
    src/ftpmanager.h \
    src/infomessage.h \
    src/irga_delegate.h \
    src/irga_desc.h \
    src/irga_model.h \
    src/irga_tableview.h \
    src/irga_view.h \
    src/mainwindow.h \
    src/mymenu.h \
    src/mystyle.h \
    src/mytabwidget.h \
    src/nonzerodoublespinbox.h \
    src/planarfitsettingsdialog.h \
    src/process.h \
    src/proxystyle.h \
    src/QProgressIndicator.h \
    src/rawfilenamedialog.h \
    src/rawfilesettingsdialog.h \
    src/runpage.h \
    src/slowmeasuretab.h \
    src/specgroup.h \
    src/splitter.h \
    src/splitterhandle.h \
    src/stringutils.h \
    src/timelagsettingsdialog.h \
    src/tooltipfilter.h \
    src/variable_delegate.h \
    src/variable_desc.h \
    src/variable_model.h \
    src/variable_tableview.h \
    src/variable_view.h \
    src/wheeleventfilter.h \
    src/smartfluxbar.h \
    src/mainwidget.h \
    src/welcomepage.h \
    src/projectpage.h \
    src/basicsettingspage.h \
    src/advancedsettingspage.h \
    src/advstatisticaloptions.h \
    src/advprocessingoptions.h \
    src/updatedialog.h \
    src/advspectraloptions.h \
    src/advoutputoptions.h \
    src/advsettingscontainer.h \
    src/createpackagedialog.h \
    src/angle_tableview.h \
    src/angle_tablemodel.h \
    src/angles_view.h \
    src/angle_item.h \
    src/ancillaryfiletest.h \
    src/make_unique.h \
    src/customcombobox.h \
    src/advmenudelegate.h \
    src/container_helpers.h
SOURCES +=  \
    src/globalsettings.cpp \
    src/widget_utils.cpp \
    src/aboutdialog.cpp \
    src/alia.cpp \
    src/anem_delegate.cpp \
    src/anem_desc.cpp \
    src/anem_model.cpp \
    src/anem_tableview.cpp \
    src/anem_view.cpp \
    src/binarysettingsdialog.cpp \
    src/biommetadatareader.cpp \
    src/clicklabel.cpp \
    src/customcombomodel.cpp \
    src/customheader.cpp \
    src/customsplashscreen.cpp \
    src/dbghelper.cpp \
    src/dlinidialog.cpp \
    src/dlinstrtab.cpp \
    src/dlproject.cpp \
    src/dlrawfiledesctab.cpp \
    src/dlsitetab.cpp \
    src/docchooser.cpp \
    src/ecproject.cpp \
    src/faderwidget.cpp \
    src/fileutils.cpp \
    src/ftpmanager.cpp \
    src/infomessage.cpp \
    src/irga_delegate.cpp \
    src/irga_desc.cpp \
    src/irga_model.cpp \
    src/irga_tableview.cpp \
    src/irga_view.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/nonzerodoublespinbox.cpp \
    src/planarfitsettingsdialog.cpp \
    src/basicsettingspage.cpp \
    src/process.cpp \
    src/proxystyle.cpp \
    src/QProgressIndicator.cpp \
    src/rawfilenamedialog.cpp \
    src/rawfilesettingsdialog.cpp \
    src/runpage.cpp \
    src/slowmeasuretab.cpp \
    src/specgroup.cpp \
    src/splitter.cpp \
    src/splitterhandle.cpp \
    src/stringutils.cpp \
    src/timelagsettingsdialog.cpp \
    src/tooltipfilter.cpp \
    src/variable_delegate.cpp \
    src/variable_desc.cpp \
    src/variable_model.cpp \
    src/variable_tableview.cpp \
    src/variable_view.cpp \
    src/wheeleventfilter.cpp \
    src/smartfluxbar.cpp \
    src/mainwidget.cpp \
    src/welcomepage.cpp \
    src/projectpage.cpp \
    src/advancedsettingspage.cpp \
    src/advstatisticaloptions.cpp \
    src/advprocessingoptions.cpp \
    src/updatedialog.cpp \
    src/advspectraloptions.cpp \
    src/advoutputoptions.cpp \
    src/advsettingscontainer.cpp \
    src/createpackagedialog.cpp \
    src/angle_tableview.cpp \
    src/angles_view.cpp \
    src/angle_tablemodel.cpp \
    src/ancillaryfiletest.cpp \
    src/defs.cpp \
    src/customcombobox.cpp \
    src/advmenudelegate.cpp
RESOURCES += eddypro.qrc
TRANSLATIONS += ./tra/eddypro_en.ts

OTHER_FILES += \
    win-files/eddypro.rc \
    win-files/eddypro.manifest \
    mac-files/eddypro.info.plist \
    css/eddypro-mac.qss \
    css/eddypro-win.qss \
    scripts/build/win-pre-link-debug.bat \
    scripts/build/win-pre-link-release.bat \
    scripts/build/mac-pre-link-debug.sh \
    scripts/build/mac-post-link-debug.sh \
    scripts/test/run-clang-analyzer.sh \
    scripts/build/mac-update-translations.sh

win32:RC_FILE += win-files/eddypro.rc
macx {
    QMAKE_INFO_PLIST = mac-files/eddypro.info.plist
    ICON = img/app.icns
}

CONFIG(release, debug|release) {
    system(lupdate -verbose -no-obsolete eddypro.pro)
    system(lrelease -verbose eddypro.pro)
}

DEFINES += QT_NO_CAST_FROM_ASCII
DEFINES += QT_NO_CAST_TO_ASCII
DEFINES += QT_NO_URL_CAST_FROM_STRING
DEFINES += QT_NO_CAST_FROM_BYTEARRAY
DEFINES += QT_USE_QSTRINGBUILDER

macx {
    static-check.target = static-check
    static-check.commands = $$_PRO_FILE_PWD_/scripts/test/run-clang-analyzer.sh
    QMAKE_EXTRA_TARGETS += static-check
}
