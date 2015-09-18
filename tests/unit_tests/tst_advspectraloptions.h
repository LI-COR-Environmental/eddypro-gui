#ifndef TST_ADVSPECTRALOPTIONS_H
#define TST_ADVSPECTRALOPTIONS_H

#include <QObject>

#include "testrunner.h"

class Test_AdvSpectralOptions_Class : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();

    void testCase1();
    void testCase2();

    void cleanupTestCase();
};

DECLARE_TEST(Test_AdvSpectralOptions_Class)

#endif // TST_ADVSPECTRALOPTIONS_H
