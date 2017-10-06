
#include <QtTest/QtTest>
#include <clocklabel.h>

class tst_ClockLabel : public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void clocklabel_data();
    void clocklabel();

};

// Subclass that exposes the protected functions.
class SubClockLabel : public ClockLabel
{
public:
};

// This will be called before the first test function is executed.
// It is only called once.
void tst_ClockLabel::initTestCase()
{
}

// This will be called after the last test function is executed.
// It is only called once.
void tst_ClockLabel::cleanupTestCase()
{
}

// This will be called before each test function is executed.
void tst_ClockLabel::init()
{
}

// This will be called after every test function.
void tst_ClockLabel::cleanup()
{
}

void tst_ClockLabel::clocklabel_data()
{
}

void tst_ClockLabel::clocklabel()
{
    SubClockLabel label;
#if 0
#endif
    QSKIP("Test is not implemented.", SkipAll);
}

QTEST_MAIN(tst_ClockLabel)
#include "tst_clocklabel.moc"

