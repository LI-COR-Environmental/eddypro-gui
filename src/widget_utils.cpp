
/***************************************************************************
  widget_utils.cpp
  -------------------
  Copyright (C) 2014, LI-COR Biosciences
  Author: Antonio Forgione

  This file is part of EddyPro (R).

  EddyPro (R) is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  EddyPro (R) is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with EddyPro (R). If not, see <http://www.gnu.org/licenses/>.
****************************************************************************/

#include "widget_utils.h"

#include <QCalendarWidget>
#include <QComboBox>
#include <QCoreApplication>
#include <QDebug>
#include <QDesktopServices>
#include <QFontMetrics>
#include <QLabel>
#include <QLineEdit>
#include <QMouseEvent>
#include <QProcess>
#include <QProgressBar>
#include <QPushButton>
#include <QSettings>
#include <QStyle>
#include <QTextEdit>
#include <QToolButton>
#include <QWidget>

#include <memory>
#include "make_unique.h"

#include "dbghelper.h"
#include "defs.h"
#include "docchooser.h"
#include "globalsettings.h"

const QColor WidgetUtils::getColor(int step)
{
    QColor c("#aae6ff");
    int h = c.hue();
    int s = c.saturation();
    int v = c.value();

    c.setHsv(abs((h - (step * 10))) % 360, s, v);

    return c;
}

namespace {

void updateStyle(QWidget* widget)
{
    // update style
    widget->style()->unpolish(widget);
    widget->style()->polish(widget);
    widget->update();
}

void updatePropertyAndStyle(QWidget* widget,
                            const char* name,
                            const QVariant& value)
{
    // set property
    widget->setProperty(name, value);

    // update the local style
    updateStyle(widget);
}

}  // namespace

void WidgetUtils::updatePropertyListAndStyle(QWidget* widget,
                                             QList<PropertyList> propertyList)
{
    foreach(const PropertyList& prop, propertyList)
    {
        // set property
        widget->setProperty(prop.first, prop.second);
    }

    // update the local style
    updateStyle(widget);
}

// NOTE: hack useful to trigger tooltip updates
void WidgetUtils::resetComboToItem(QComboBox* combo, int i)
{
    combo->setCurrentIndex(-1);
    combo->setCurrentIndex(i);
}

// set the combo tooltip to the specified item tooltip
void WidgetUtils::updateComboItemTooltip(QComboBox* combo, int i)
{
    combo->setToolTip(combo->itemData(i, Qt::ToolTipRole).toString());
}

// set the line edit tooltip to the current line edit text
void WidgetUtils::updateLineEditToolip(QLineEdit* lineedit)
{
    lineedit->setToolTip(QString(QStringLiteral("%1")).arg(lineedit->text()));
}

// return the data of the current combo item
QVariant WidgetUtils::currentComboItemData(QComboBox* combo, int role)
{
    return combo->itemData(combo->currentIndex(), role);
}

// remove the context help button from the widget
// (tipically a dialog or message box)
void WidgetUtils::removeContextHelpButton(QWidget* w)
{
    Qt::WindowFlags winFflags = w->windowFlags();
    winFflags &= ~Qt::WindowContextHelpButtonHint;
    w->setWindowFlags(winFflags);
}

// set the text of the label with specified elide mode and width
void WidgetUtils::setElidedTextToLabel(QLabel* label,
                                       const QString& text,
                                       Qt::TextElideMode mode,
                                       int width)
{
    QFontMetrics fm(label->font());
    int realWidth = width - 40;
    QString clippedText = fm.elidedText(text, mode, realWidth);
    label->setText(clippedText);
}

// set the text of the line edit with specified elide mode and width
void WidgetUtils::setElidedTextToLineEdit(QLineEdit* lineEdit,
                                          const QString& text,
                                          Qt::TextElideMode mode,
                                          int width)
{
    QFontMetrics fm(lineEdit->font());
    int realWidth = width - 40;
    QString clippedText = fm.elidedText(text, mode, realWidth);
    lineEdit->setText(clippedText);
}

void WidgetUtils::customizeCalendar(QCalendarWidget* cal)
{
    cal->setVerticalHeaderFormat(QCalendarWidget::ISOWeekNumbers);

    QToolButton *btn = cal->findChild<QToolButton *>(QStringLiteral("qt_calendar_prevmonth"));
    QIcon icon;
    icon.addPixmap(QPixmap(QStringLiteral(":/icons/cal-left-arrow")), QIcon::Normal, QIcon::On);
    btn->setIcon(icon);

    btn = cal->findChild<QToolButton *>(QStringLiteral("qt_calendar_nextmonth"));
    icon.addPixmap(QPixmap(QStringLiteral(":/icons/cal-right-arrow")), QIcon::Normal, QIcon::On);
    btn->setIcon(icon);

    cal->setStyleSheet(QStringLiteral(
    "QCalendarView {border: 1px solid #666; padding: 3px;}"
    "QAbstractItemView {selection-background-color: #318ef4;}"
    "QToolButton:hover {color: black; background: #2B7AE0;}"
    "QToolButton#qt_calendar_monthbutton {padding-right: 10px;}"
    "QToolButton::menu-indicator {"
                           "width: 18px; height: 20px;"
                           "image: url(:/icons/cal-down);}"
    "QToolButton::menu-indicator:pressed,"
    "QToolButton::menu-indicator:open { /*top:10px; left: 10px;*/"
                           "image: url(:/icons/cal-down-pressed);}"));
}

// NOTE: hack to show the calendar,
// because dateEdit->calendarWidget()->show() does not work
void WidgetUtils::showCalendarOf(QWidget* widget)
{
    QCoreApplication::postEvent(widget,
                                new QMouseEvent(QEvent::MouseButtonPress,
                                                QPoint(100.0, 10.0),
                                                Qt::LeftButton,
                                                Qt::NoButton,
                                                Qt::NoModifier));
}

// NOTE: hack to add <hr> to QTextEdit. Not working as expected.
// See https://bugreports.qt-project.org/browse/QTBUG-747
void WidgetUtils::appendHrToTextEdit(QTextEdit* te)
{
    QTextCursor textCursor = te->textCursor();
    QTextBlockFormat blockFmt = textCursor.blockFormat();
    te->append(QStringLiteral("<hr>"));
    te->textCursor().setBlockFormat(blockFmt);
}

void WidgetUtils::openAppWebsite()
{
    QDesktopServices::openUrl(QUrl(Defs::APP_WEBSITE));
}

bool WidgetUtils::okToOverwrite(const QString& filename)
{
    return okToQuestion(nullptr,
                    QObject::tr("Overwrite File"),
                    QObject::tr("<p>Are you sure you want to "
                                "overwrite the following file?</p>"),
                    QObject::tr("<p><b>\"%1\"</b></p>").arg(filename),
                    QObject::tr("Yes"),
                    QObject::tr("Cancel"));
}

bool WidgetUtils::okToRemoveColumn(QWidget* parent)
{
    return okToQuestion(parent,
                    QObject::tr("Remove Column"),
                    QObject::tr("Do you want to remove this column?"));
}

QMessageBox::ButtonRole WidgetUtils::requestToSave(QWidget* parent,
                                                   const QString& title,
                                                   const QString& text,
                                                   const QString& infoText)
{
    auto messageBox = std::make_unique<QMessageBox>(parent);

    // Mac OS X compatibility (to look like a sheet)
    if (parent)
    {
        messageBox->setWindowModality(Qt::WindowModal);
    }
    messageBox->setIconPixmap(QStringLiteral(":/icons/msg-question"));
    messageBox->setWindowTitle(title);
    messageBox->setText(text);
    if (!infoText.isEmpty())
    {
        messageBox->setInformativeText(infoText);
    }
    messageBox->addButton(QMessageBox::Save);
    messageBox->addButton(QObject::tr("Do &Not Save"), QMessageBox::RejectRole);
    messageBox->addButton(QMessageBox::Cancel);
    messageBox->setEscapeButton(QMessageBox::Cancel);
    messageBox->setDefaultButton(QMessageBox::Save);

    WidgetUtils::removeContextHelpButton(messageBox.get());

    messageBox->exec();
    return messageBox->buttonRole(messageBox->clickedButton());
}

bool WidgetUtils::information(QWidget* parent,
                              const QString& title,
                              const QString& text,
                              const QString& infoText)
{
//    QScopedPointer<QMessageBox> messageBox(new QMessageBox(parent));
    auto messageBox = std::make_unique<QMessageBox>(parent);

    // Mac OS X compatibility (to look like a sheet)
    if (parent)
    {
        messageBox->setWindowModality(Qt::WindowModal);
    }
    messageBox->setWindowTitle(title);
    messageBox->setText(text);
    if (!infoText.isEmpty())
    {
        messageBox->setInformativeText(infoText);
    }
    messageBox->setIconPixmap(QStringLiteral(":/icons/msg-info"));
    messageBox->addButton(QMessageBox::Ok);
    messageBox->setEscapeButton(QMessageBox::Ok);

//    WidgetUtils::removeContextHelpButton(messageBox.data());
    WidgetUtils::removeContextHelpButton(messageBox.get());

    return messageBox->exec();
}

void WidgetUtils::warning(QWidget* parent,
                          const QString& title,
                          const QString& text,
                          const QString& infoText)
{
    auto messageBox = std::make_unique<QMessageBox>(parent);

    // Mac OS X compatibility (to look like a sheet)
    if (parent)
    {
        messageBox->setWindowModality(Qt::WindowModal);
    }
    messageBox->setWindowTitle(title);
    messageBox->setText(text);
    if (!infoText.isEmpty())
    {
        messageBox->setInformativeText(infoText);
    }
    messageBox->setIconPixmap(QStringLiteral(":/icons/msg-warning"));
    messageBox->addButton(QMessageBox::Ok);
    messageBox->setEscapeButton(QMessageBox::Ok);

    WidgetUtils::removeContextHelpButton(messageBox.get());

    messageBox->exec();
}

void WidgetUtils::critical(QWidget* parent,
                           const QString& title,
                           const QString& text,
                           const QString& infoText)
{
    auto messageBox = std::make_unique<QMessageBox>(parent);

    // Mac OS X compatibility (to look like a sheet)
    if (parent)
    {
        messageBox->setWindowModality(Qt::WindowModal);
    }
    messageBox->setWindowTitle(title);
    messageBox->setText(text);
    if (!infoText.isEmpty())
    {
        messageBox->setInformativeText(infoText);
    }
    messageBox->setIconPixmap(QStringLiteral(":/icons/msg-critical"));
    messageBox->addButton(QMessageBox::Ok);
    messageBox->setEscapeButton(QMessageBox::Ok);

    WidgetUtils::removeContextHelpButton(messageBox.get());

    messageBox->exec();
}

bool WidgetUtils::okToQuestion(QWidget* parent,
                           const QString& title,
                           const QString& text,
                           const QString& infoText,
                           const QString& yesText,
                           const QString& noText)
{
    auto messageBox = std::make_unique<QMessageBox>(parent);

    // Mac OS X compatibility (to look like a sheet)
    if (parent)
    {
        messageBox->setWindowModality(Qt::WindowModal);
    }
    messageBox->setWindowTitle(title);
    messageBox->setText(text);
    if (!infoText.isEmpty())
    {
        messageBox->setInformativeText(infoText);
    }
    messageBox->setIconPixmap(QStringLiteral(":/icons/msg-question"));
    QPushButton *yesButton = messageBox->addButton(yesText,
            QMessageBox::AcceptRole);
    QPushButton* noButton = messageBox->addButton(noText, QMessageBox::RejectRole);
    messageBox->setEscapeButton(noButton);
    messageBox->setDefaultButton(yesButton);

    WidgetUtils::removeContextHelpButton(messageBox.get());

    messageBox->exec();
    return (messageBox->clickedButton() == yesButton);
}

namespace {

// http://stackoverflow.com/questions/2404449/process-starturl-with-anchor-in-the-url
bool launchWinWebBrowser(const QUrl& url)
{
    DEBUG_FUNC_NAME

    QString defaultBrowserKey;
    QString defaultBrowserPath;

    QSettings settings_1(QStringLiteral("HKEY_CLASSES_ROOT\\.htm\\OpenWithProgIDs"),
                        QSettings::NativeFormat);
    QStringList keys = settings_1.allKeys();

    if (!keys.isEmpty() && keys.count() > 1)
    {
        keys.removeAll(QStringLiteral("Default"));

        defaultBrowserKey = QStringLiteral("HKEY_CLASSES_ROOT\\")
                            + keys.first()
                            + QStringLiteral("\\shell\\open\\command");
        qDebug() << "defaultBrowserKey" << defaultBrowserKey;

        QSettings settings_2(defaultBrowserKey,
                            QSettings::NativeFormat);
        defaultBrowserPath = settings_2.value(QStringLiteral("Default")).toString();
    }
    else
    {
        QSettings settings_3(QStringLiteral("HKEY_CLASSES_ROOT\\http\\shell\\open\\command"),
                        QSettings::NativeFormat);
        defaultBrowserPath = settings_3.value(QStringLiteral("Default")).toString();

        if (defaultBrowserPath.isEmpty())
        {
            QSettings settings_3(QStringLiteral("HKCU\\Software\\Microsoft\\Windows"
                                                "\\Shell\\Associations\\UrlAssociations\\http\\UserChoice"),
                            QSettings::NativeFormat);
            QString progId = settings_3.value(QStringLiteral("Progid")).toString();

            defaultBrowserKey = QStringLiteral("HKEY_CLASSES_ROOT\\")
                                + progId
                                + QStringLiteral("\\shell\\open\\command");
            qDebug() << "defaultBrowserKey" << defaultBrowserKey;

            QSettings settings_4(defaultBrowserKey,
                                QSettings::NativeFormat);
            defaultBrowserPath = settings_4.value(QStringLiteral("Default")).toString();
        }
    }

    if (!defaultBrowserPath.isEmpty())
    {
        qDebug() << "defaultBrowserPath"
                 << defaultBrowserPath;
        defaultBrowserPath.chop(1);
        defaultBrowserPath = defaultBrowserPath.remove(0, 1).split(QLatin1Char('"')).at(0);
        qDebug() << "defaultBrowserPath"
                 << defaultBrowserPath;
    }
    else
    {
        defaultBrowserPath = QStringLiteral("C:\\Program Files (x86)\\Internet Explorer\\iexplore.exe");
    }

    if (QProcess::startDetached(defaultBrowserPath, QStringList() << url.toString()))
        return true;
    else
        return false;
}

}  // namespace

void WidgetUtils::showHelp(const QUrl& url)
{
    // read state
    bool autoChooseHelp = GlobalSettings::getAppPersistentSettings(
                Defs::CONFGROUP_WINDOW,
                Defs::CONF_WIN_AUTOHELP, false).toBool();
    bool offlineHelp = GlobalSettings::getAppPersistentSettings(
                Defs::CONFGROUP_WINDOW,
                Defs::CONF_WIN_OFFLINEHELP, false).toBool();

    if (autoChooseHelp)
    {
        if (!offlineHelp)
        {
            // browse online help version
            qDebug() << "url" << url << QDesktopServices::openUrl(url);
        }
        else
        {
            // open local help
            QString htmlHelpPath = qApp->applicationDirPath();
            QString localUrlString = QString();

            if (url.toString().contains(QStringLiteral("EddyPro_Home")))
            {
                 htmlHelpPath = htmlHelpPath + QStringLiteral("/docs/help/index.htm#EddyPro_Home.htm");
            }
            else if (url.toString().contains(QStringLiteral("Getting_Started")))
            {
                 htmlHelpPath = htmlHelpPath + QStringLiteral("/docs/EddyPro5_Getting_Started.pdf");
            }
            else if (url.toString().contains(QStringLiteral("User_Guide")))
            {
                 htmlHelpPath = htmlHelpPath + QStringLiteral("/docs/EddyPro5_User_Guide.pdf");
            }
            else if (url.toString().contains(QStringLiteral("Video_Library")))
            {
                 htmlHelpPath = htmlHelpPath + QStringLiteral("/docs/help/index.htm#Video_Library.htm");
            }
            else
            {
                localUrlString = url.toString(QUrl::RemoveAuthority
                    | QUrl::RemoveScheme).remove(QStringLiteral("/EddyPro5"));
                qDebug() << "localUrlString" << localUrlString;

                htmlHelpPath = htmlHelpPath + QString(QStringLiteral("/docs")) + localUrlString;
            }

            QUrl localUrl = QUrl();

            qDebug() << "htmlHelpPath" << htmlHelpPath;
            if (htmlHelpPath.contains(QStringLiteral("#")))
            {
                QString localUrlHost = htmlHelpPath.section(QLatin1Char('#'), 0, 0);
                QString localUrlFragment = htmlHelpPath.section(QLatin1Char('#'), 1, 1);

                qDebug() << "localUrlFragment" << localUrlFragment;

                localUrl = QUrl::fromLocalFile(localUrlHost);
                qDebug() << "localUrl" << localUrl;
                localUrl.setFragment(localUrlFragment);
                qDebug() << "localUrl.setFragment" << localUrl;
            }
            else
            {
                localUrl = QUrl::fromLocalFile(htmlHelpPath);
                qDebug() << "localUrl with no hash" << localUrl;
            }

#if defined(Q_OS_WIN)
            qDebug() << "localUrl osWin" << localUrl << launchWinWebBrowser(localUrl);
#else
            qDebug() << "localUrl" << localUrl << QDesktopServices::openUrl(localUrl);
#endif
        }
    }
    else
    {
        DocChooserDialog docChooser(url);
        docChooser.exec();
    }
}

void WidgetUtils::setProgressValue(QProgressBar* bar, int value)
{
    int boundValue = qBound(bar->minimum(),
                            value,
                            bar->maximum());
    bar->setValue(boundValue);
}
