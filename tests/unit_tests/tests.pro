QT += core gui widgets testlib

TARGET = unit_tests

CONFIG += console
CONFIG -= app_bundle
CONFIG += c++11

TEMPLATE = app

include(../QtTestUtil/QtTestUtil.pri)

HEADERS += \
    tst_advspectraloptions.h \
#    testrunner.h \
    tst_aboutdialog.h

SOURCES += \
    tst_advspectraloptions.cpp \
    main.cpp \
    tst_aboutdialog.cpp
#    tst_aboutdialog_s.cpp

INCLUDEPATH += $$top_srcdir/src
message(top_srcdir: $$top_srcdir)

QMAKE_MAC_SDK = macosx10.11
