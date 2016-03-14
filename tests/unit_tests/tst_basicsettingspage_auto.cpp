
#include <QtTest/QtTest>
#include <basicsettingspage.h>

class tst_BasicSettingsPage : public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void basicsettingspage_data();
    void basicsettingspage();

    void alignDeclinationDate_data();
    void alignDeclinationDate();
    void askRawFilenamePrototype_data();
    void askRawFilenamePrototype();
    void clearFilesFound_data();
    void clearFilesFound();
    void clearSelectedItems_data();
    void clearSelectedItems();
    void datapathSelected_data();
    void datapathSelected();
    void outpathBrowseSelected_data();
    void outpathBrowseSelected();
    void partialRefresh_data();
    void partialRefresh();
    void previousDatapathSelected_data();
    void previousDatapathSelected();
    void refresh_data();
    void refresh();
    void showSetPrototype_data();
    void showSetPrototype();
    void updateMetadataRead_data();
    void updateMetadataRead();
    void updateProjectFilesFound_data();
    void updateProjectFilesFound();
    void updateSmartfluxBar_data();
    void updateSmartfluxBar();
    void saveSilentlyRequest_data();
    void saveSilentlyRequest();
    void setDateRangeRequest_data();
    void setDateRangeRequest();
    void updateMetadataReadResult_data();
    void updateMetadataReadResult();
};

// Subclass that exposes the protected functions.
class SubBasicSettingsPage : public BasicSettingsPage
{
public:
    void call_saveSilentlyRequest()
        { return SubBasicSettingsPage::saveSilentlyRequest(); }

    void call_setDateRangeRequest(QPair<QDateTime,QDateTime> arg0)
        { return SubBasicSettingsPage::setDateRangeRequest(arg0); }

    void call_updateMetadataReadResult(bool b)
        { return SubBasicSettingsPage::updateMetadataReadResult(b); }

};

// This will be called before the first test function is executed.
// It is only called once.
void tst_BasicSettingsPage::initTestCase()
{
}

// This will be called after the last test function is executed.
// It is only called once.
void tst_BasicSettingsPage::cleanupTestCase()
{
}

// This will be called before each test function is executed.
void tst_BasicSettingsPage::init()
{
}

// This will be called after every test function.
void tst_BasicSettingsPage::cleanup()
{
}

void tst_BasicSettingsPage::basicsettingspage_data()
{
}

void tst_BasicSettingsPage::basicsettingspage()
{
    SubBasicSettingsPage page;
#if 0
    page.alignDeclinationDate(QDate());
    page.askRawFilenamePrototype();
    page.clearFilesFound();
    page.clearSelectedItems();
    page.datapathSelected(QString());
    page.outpathBrowseSelected(QString());
    page.partialRefresh();
    page.previousDatapathSelected(QString());
    page.refresh();
    page.showSetPrototype();
    page.updateMetadataRead(false);
    page.updateProjectFilesFound(-1);
    page.updateSmartfluxBar();
    page.call_saveSilentlyRequest();
    page.call_setDateRangeRequest(QPair<QDateTime,QDateTime>);
    page.call_updateMetadataReadResult(false);
#endif
    QSKIP("Test is not implemented.", SkipAll);
}

Q_DECLARE_METATYPE(QDate)
void tst_BasicSettingsPage::alignDeclinationDate_data()
{
#if 0
    QTest::addColumn<QDate>("d");
    QTest::newRow("null") << QDate();
#endif
}

// public void alignDeclinationDate(QDate const& d)
void tst_BasicSettingsPage::alignDeclinationDate()
{
#if 0
    QFETCH(QDate, d);

    SubBasicSettingsPage page;

    QSignalSpy spy0(&page, SIGNAL(saveSilentlyRequest()));
    QSignalSpy spy1(&page, SIGNAL(setDateRangeRequest(QPair<QDateTime,QDateTime>)));
    QSignalSpy spy2(&page, SIGNAL(updateMetadataReadResult(bool)));

    page.alignDeclinationDate(d);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
#endif
    QSKIP("Test is not implemented.", SkipAll);
}

void tst_BasicSettingsPage::askRawFilenamePrototype_data()
{
    QTest::addColumn<int>("foo");
    QTest::newRow("0") << 0;
    QTest::newRow("-1") << -1;
}

// public void askRawFilenamePrototype()
void tst_BasicSettingsPage::askRawFilenamePrototype()
{
#if 0
    QFETCH(int, foo);

    SubBasicSettingsPage page;

    QSignalSpy spy0(&page, SIGNAL(saveSilentlyRequest()));
    QSignalSpy spy1(&page, SIGNAL(setDateRangeRequest(QPair<QDateTime,QDateTime>)));
    QSignalSpy spy2(&page, SIGNAL(updateMetadataReadResult(bool)));

    page.askRawFilenamePrototype();

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
#endif
    QSKIP("Test is not implemented.", SkipAll);
}

void tst_BasicSettingsPage::clearFilesFound_data()
{
    QTest::addColumn<int>("foo");
    QTest::newRow("0") << 0;
    QTest::newRow("-1") << -1;
}

// public void clearFilesFound()
void tst_BasicSettingsPage::clearFilesFound()
{
#if 0
    QFETCH(int, foo);

    SubBasicSettingsPage page;

    QSignalSpy spy0(&page, SIGNAL(saveSilentlyRequest()));
    QSignalSpy spy1(&page, SIGNAL(setDateRangeRequest(QPair<QDateTime,QDateTime>)));
    QSignalSpy spy2(&page, SIGNAL(updateMetadataReadResult(bool)));

    page.clearFilesFound();

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
#endif
    QSKIP("Test is not implemented.", SkipAll);
}

void tst_BasicSettingsPage::clearSelectedItems_data()
{
    QTest::addColumn<int>("foo");
    QTest::newRow("0") << 0;
    QTest::newRow("-1") << -1;
}

// public void clearSelectedItems()
void tst_BasicSettingsPage::clearSelectedItems()
{
#if 0
    QFETCH(int, foo);

    SubBasicSettingsPage page;

    QSignalSpy spy0(&page, SIGNAL(saveSilentlyRequest()));
    QSignalSpy spy1(&page, SIGNAL(setDateRangeRequest(QPair<QDateTime,QDateTime>)));
    QSignalSpy spy2(&page, SIGNAL(updateMetadataReadResult(bool)));

    page.clearSelectedItems();

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
#endif
    QSKIP("Test is not implemented.", SkipAll);
}

void tst_BasicSettingsPage::datapathSelected_data()
{
    QTest::addColumn<QString>("dir_path");
    QTest::newRow("null") << QString();
    QTest::newRow("foo") << QString("foo");
}

// public void datapathSelected(QString const& dir_path)
void tst_BasicSettingsPage::datapathSelected()
{
#if 0
    QFETCH(QString, dir_path);

    SubBasicSettingsPage page;

    QSignalSpy spy0(&page, SIGNAL(saveSilentlyRequest()));
    QSignalSpy spy1(&page, SIGNAL(setDateRangeRequest(QPair<QDateTime,QDateTime>)));
    QSignalSpy spy2(&page, SIGNAL(updateMetadataReadResult(bool)));

    page.datapathSelected(dir_path);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
#endif
    QSKIP("Test is not implemented.", SkipAll);
}

void tst_BasicSettingsPage::outpathBrowseSelected_data()
{
    QTest::addColumn<QString>("dir_path");
    QTest::newRow("null") << QString();
    QTest::newRow("foo") << QString("foo");
}

// public void outpathBrowseSelected(QString const& dir_path)
void tst_BasicSettingsPage::outpathBrowseSelected()
{
#if 0
    QFETCH(QString, dir_path);

    SubBasicSettingsPage page;

    QSignalSpy spy0(&page, SIGNAL(saveSilentlyRequest()));
    QSignalSpy spy1(&page, SIGNAL(setDateRangeRequest(QPair<QDateTime,QDateTime>)));
    QSignalSpy spy2(&page, SIGNAL(updateMetadataReadResult(bool)));

    page.outpathBrowseSelected(dir_path);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
#endif
    QSKIP("Test is not implemented.", SkipAll);
}

void tst_BasicSettingsPage::partialRefresh_data()
{
    QTest::addColumn<int>("foo");
    QTest::newRow("0") << 0;
    QTest::newRow("-1") << -1;
}

// public void partialRefresh()
void tst_BasicSettingsPage::partialRefresh()
{
#if 0
    QFETCH(int, foo);

    SubBasicSettingsPage page;

    QSignalSpy spy0(&page, SIGNAL(saveSilentlyRequest()));
    QSignalSpy spy1(&page, SIGNAL(setDateRangeRequest(QPair<QDateTime,QDateTime>)));
    QSignalSpy spy2(&page, SIGNAL(updateMetadataReadResult(bool)));

    page.partialRefresh();

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
#endif
    QSKIP("Test is not implemented.", SkipAll);
}

void tst_BasicSettingsPage::previousDatapathSelected_data()
{
    QTest::addColumn<QString>("dir_path");
    QTest::newRow("null") << QString();
    QTest::newRow("foo") << QString("foo");
}

// public void previousDatapathSelected(QString const& dir_path)
void tst_BasicSettingsPage::previousDatapathSelected()
{
#if 0
    QFETCH(QString, dir_path);

    SubBasicSettingsPage page;

    QSignalSpy spy0(&page, SIGNAL(saveSilentlyRequest()));
    QSignalSpy spy1(&page, SIGNAL(setDateRangeRequest(QPair<QDateTime,QDateTime>)));
    QSignalSpy spy2(&page, SIGNAL(updateMetadataReadResult(bool)));

    page.previousDatapathSelected(dir_path);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
#endif
    QSKIP("Test is not implemented.", SkipAll);
}

void tst_BasicSettingsPage::refresh_data()
{
    QTest::addColumn<int>("foo");
    QTest::newRow("0") << 0;
    QTest::newRow("-1") << -1;
}

// public void refresh()
void tst_BasicSettingsPage::refresh()
{
#if 0
    QFETCH(int, foo);

    SubBasicSettingsPage page;

    QSignalSpy spy0(&page, SIGNAL(saveSilentlyRequest()));
    QSignalSpy spy1(&page, SIGNAL(setDateRangeRequest(QPair<QDateTime,QDateTime>)));
    QSignalSpy spy2(&page, SIGNAL(updateMetadataReadResult(bool)));

    page.refresh();

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
#endif
    QSKIP("Test is not implemented.", SkipAll);
}

void tst_BasicSettingsPage::showSetPrototype_data()
{
    QTest::addColumn<int>("foo");
    QTest::newRow("0") << 0;
    QTest::newRow("-1") << -1;
}

// public void showSetPrototype()
void tst_BasicSettingsPage::showSetPrototype()
{
#if 0
    QFETCH(int, foo);

    SubBasicSettingsPage page;

    QSignalSpy spy0(&page, SIGNAL(saveSilentlyRequest()));
    QSignalSpy spy1(&page, SIGNAL(setDateRangeRequest(QPair<QDateTime,QDateTime>)));
    QSignalSpy spy2(&page, SIGNAL(updateMetadataReadResult(bool)));

    page.showSetPrototype();

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
#endif
    QSKIP("Test is not implemented.", SkipAll);
}

void tst_BasicSettingsPage::updateMetadataRead_data()
{
    QTest::addColumn<bool>("firstReading");
    QTest::newRow("true") << true;
    QTest::newRow("false") << false;
}

// public void updateMetadataRead(bool firstReading = false)
void tst_BasicSettingsPage::updateMetadataRead()
{
#if 0
    QFETCH(bool, firstReading);

    SubBasicSettingsPage page;

    QSignalSpy spy0(&page, SIGNAL(saveSilentlyRequest()));
    QSignalSpy spy1(&page, SIGNAL(setDateRangeRequest(QPair<QDateTime,QDateTime>)));
    QSignalSpy spy2(&page, SIGNAL(updateMetadataReadResult(bool)));

    page.updateMetadataRead(firstReading);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
#endif
    QSKIP("Test is not implemented.", SkipAll);
}

void tst_BasicSettingsPage::updateProjectFilesFound_data()
{
    QTest::addColumn<int>("fileNumber");
    QTest::newRow("0") << 0;
    QTest::newRow("-1") << -1;
}

// public void updateProjectFilesFound(int fileNumber)
void tst_BasicSettingsPage::updateProjectFilesFound()
{
#if 0
    QFETCH(int, fileNumber);

    SubBasicSettingsPage page;

    QSignalSpy spy0(&page, SIGNAL(saveSilentlyRequest()));
    QSignalSpy spy1(&page, SIGNAL(setDateRangeRequest(QPair<QDateTime,QDateTime>)));
    QSignalSpy spy2(&page, SIGNAL(updateMetadataReadResult(bool)));

    page.updateProjectFilesFound(fileNumber);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
#endif
    QSKIP("Test is not implemented.", SkipAll);
}

void tst_BasicSettingsPage::updateSmartfluxBar_data()
{
    QTest::addColumn<int>("foo");
    QTest::newRow("0") << 0;
    QTest::newRow("-1") << -1;
}

// public void updateSmartfluxBar()
void tst_BasicSettingsPage::updateSmartfluxBar()
{
#if 0
    QFETCH(int, foo);

    SubBasicSettingsPage page;

    QSignalSpy spy0(&page, SIGNAL(saveSilentlyRequest()));
    QSignalSpy spy1(&page, SIGNAL(setDateRangeRequest(QPair<QDateTime,QDateTime>)));
    QSignalSpy spy2(&page, SIGNAL(updateMetadataReadResult(bool)));

    page.updateSmartfluxBar();

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
#endif
    QSKIP("Test is not implemented.", SkipAll);
}

void tst_BasicSettingsPage::saveSilentlyRequest_data()
{
    QTest::addColumn<int>("foo");
    QTest::newRow("0") << 0;
    QTest::newRow("-1") << -1;
}

// protected void saveSilentlyRequest()
void tst_BasicSettingsPage::saveSilentlyRequest()
{
#if 0
    QFETCH(int, foo);

    SubBasicSettingsPage page;

    QSignalSpy spy0(&page, SIGNAL(saveSilentlyRequest()));
    QSignalSpy spy1(&page, SIGNAL(setDateRangeRequest(QPair<QDateTime,QDateTime>)));
    QSignalSpy spy2(&page, SIGNAL(updateMetadataReadResult(bool)));

    page.call_saveSilentlyRequest();

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
#endif
    QSKIP("Test is not implemented.", SkipAll);
}

Q_DECLARE_METATYPE(QPair<QDateTime,QDateTime>)
void tst_BasicSettingsPage::setDateRangeRequest_data()
{
#if 0
    QTest::addColumn<QPair<QDateTime,QDateTime>>("");
    QTest::newRow("null") << QPair<QDateTime,QDateTime>();
#endif
}

// protected void setDateRangeRequest(QPair<QDateTime,QDateTime> arg0)
void tst_BasicSettingsPage::setDateRangeRequest()
{
#if 0
    QFETCH(QPair<QDateTime,QDateTime>, );

    SubBasicSettingsPage page;

    QSignalSpy spy0(&page, SIGNAL(saveSilentlyRequest()));
    QSignalSpy spy1(&page, SIGNAL(setDateRangeRequest(QPair<QDateTime,QDateTime>)));
    QSignalSpy spy2(&page, SIGNAL(updateMetadataReadResult(bool)));

    page.call_setDateRangeRequest();

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
#endif
    QSKIP("Test is not implemented.", SkipAll);
}

void tst_BasicSettingsPage::updateMetadataReadResult_data()
{
    QTest::addColumn<bool>("b");
    QTest::newRow("true") << true;
    QTest::newRow("false") << false;
}

// protected void updateMetadataReadResult(bool b)
void tst_BasicSettingsPage::updateMetadataReadResult()
{
#if 0
    QFETCH(bool, b);

    SubBasicSettingsPage page;

    QSignalSpy spy0(&page, SIGNAL(saveSilentlyRequest()));
    QSignalSpy spy1(&page, SIGNAL(setDateRangeRequest(QPair<QDateTime,QDateTime>)));
    QSignalSpy spy2(&page, SIGNAL(updateMetadataReadResult(bool)));

    page.call_updateMetadataReadResult(b);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
#endif
    QSKIP("Test is not implemented.", SkipAll);
}

QTEST_MAIN(tst_BasicSettingsPage)
#include "tst_basicsettingspage.moc"

