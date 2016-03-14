
#include <QtTest/QtTest>
#include <aboutdialog.h>

class tst_AboutDialog : public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void aboutdialog_data();
    void aboutdialog();

};

// Subclass that exposes the protected functions.
class SubAboutDialog : public AboutDialog
{
public:
};

// This will be called before the first test function is executed.
// It is only called once.
void tst_AboutDialog::initTestCase()
{
}

// This will be called after the last test function is executed.
// It is only called once.
void tst_AboutDialog::cleanupTestCase()
{
}

// This will be called before each test function is executed.
void tst_AboutDialog::init()
{
}

// This will be called after every test function.
void tst_AboutDialog::cleanup()
{
}

void tst_AboutDialog::aboutdialog_data()
{
}

void tst_AboutDialog::aboutdialog()
{
    SubAboutDialog dialog;
#if 0
#endif
    QSKIP("Test is not implemented.", SkipAll);
}

QTEST_MAIN(tst_AboutDialog)
#include "tst_aboutdialog.moc"

