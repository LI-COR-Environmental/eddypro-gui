#include <QObject>
#include <QTest>

#include "QtTestUtil/QtTestUtil.h"

#include "aboutdialog.h"

class Test_AboutDialog_Class : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase() {}

    void testCase1()
    {
        QVERIFY2(true, "Failure");
    }
    void testCase2()
    {
        QCOMPARE(1, 3); // Dummy test
    }

    void cleanupTestCase() {}
};

QTTESTUTIL_REGISTER_TEST(Test_AboutDialog_Class);
