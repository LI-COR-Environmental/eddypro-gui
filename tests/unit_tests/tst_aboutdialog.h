#ifndef TST_ABOUTDIALOG_H
#define TST_ABOUTDIALOG_H

#include <QObject>

//#include "testrunner.h"

#include "QtTestUtil/QtTestUtil.h"

class Test_AboutDialog_Class : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();

    void testCase1();
    void testCase2();

    void cleanupTestCase();
};

//DECLARE_TEST(Test_AboutDialog_Class)
//QTTESTUTIL_REGISTER_TEST(Test_AboutDialog_Class);

#endif // TST_ABOUTDIALOG_H
