/***************************************************************************
  alia.cpp
  -------------------
  Copyright (C) 2007-2011, Eco2s team, Antonio Forgione
  Copyright (C) 2011-2014, LI-COR Biosciences
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

#include <QProcess>
#include <QSettings>
#include <QMouseEvent>
#include <QComboBox>
#include <QStyle>
#include <QUrl>
#include <QDesktopServices>
#include <QNetworkConfigurationManager>
#include <QTcpSocket>
#include <QNetworkProxyFactory>
#include <QCalendarWidget>
#include <QToolButton>
#include <QPushButton>
#include <QMessageBox>
#include <QLabel>
#include <QLineEdit>
#include <QDebug>
#include <QApplication>

#include "docchooser.h"
#include "defs.h"
#include "dbghelper.h"
#include "variable_desc.h"
#include "infomessage.h"
#include "alia.h"

void Alia::updateStyle(QWidget* widget)
{
    // update style
    widget->style()->unpolish(widget);
    widget->style()->polish(widget);
    widget->update();
}

void Alia::updatePropertyAndStyle(QWidget* widget, const char* name, const QVariant& value)
{
    // set property
    widget->setProperty(name, value);

    // update the local style
    Alia::updateStyle(widget);
}

void Alia::updatePropertyListAndStyle(QWidget* widget, QList<PropertyList> propertyList)
{
    foreach (const PropertyList& prop, propertyList)
    {
        // set property
        widget->setProperty(prop.first, prop.second);
    }

    // update the local style
    Alia::updateStyle(widget);
}

bool Alia::isOnline()
{
    return false;
}

bool Alia::isOnline_2()
{
    QNetworkConfigurationManager ifConfigManager;
    return ifConfigManager.isOnline();
}

void Alia::showHelp(const QUrl& url)
{
    QSettings config;
    bool autoChooseHelp = false;
    bool offlineHelp = false;

    // read state
    config.beginGroup(Defs::CONFGROUP_WINDOW);
        autoChooseHelp = config.value(Defs::CONF_WIN_AUTOHELP).toBool();
        offlineHelp = config.value(Defs::CONF_WIN_OFFLINEHELP).toBool();
    config.endGroup();

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
            QString htmlHelpPath = QCoreApplication::applicationDirPath();
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
            qDebug() << "localUrl osWin" << localUrl << launchWebBrowser(localUrl);
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

void Alia::customizeCalendar(QCalendarWidget* cal)
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
                           "image: url(:/icons/cal-down-pressed);}"
    ));
}

int Alia::queryEcReset_1()
{
    return QMessageBox::question(0,
                            tr("Reset Project"),
                            tr("<p>Do you want to reset all the Advanced Settings "
                               "to the default settings?</p>"
                               "<p>You cannot undo this action.</p>"),
                            QMessageBox::Yes | QMessageBox::Cancel,
                            QMessageBox::Cancel);
}

int Alia::queryEcReset_2()
{
    return QMessageBox::question(0,
                            tr("Reset Project"),
                            tr("<p>Do you want to reset the surface heating correction "
                               "to the default values of Burba et al. (2008)?</p>"
                               "<p>You cannot undo this action.</p>"),
                            QMessageBox::Yes | QMessageBox::Cancel,
                            QMessageBox::Cancel);
}

int Alia::queryEcReset_3()
{
    return QMessageBox::question(0,
                            tr("Reset Project"),
                            tr("<p>Do you want to reset the Statistical Analysis "
                               "settings to the default settings?</p>"
                               "<p>You cannot undo this action.</p>"),
                            QMessageBox::Yes | QMessageBox::Cancel,
                            QMessageBox::Cancel);
}

int Alia::queryMdReset()
{
    return QMessageBox::question(0,
                            tr("Reset Metadata"),
                            tr("<p>Are you sure you want to "
                               "reset the current metadata values?</p>"
                               "<p>You cannot undo this action.</p>"),
                            QMessageBox::Yes | QMessageBox::Cancel,
                            QMessageBox::Cancel);
}

int Alia::queryOverwrite(const QString& filename)
{
    return QMessageBox::question(0,
                            tr("Overwrite File"),
                            tr("<p>Are you sure you want to "
                               "overwrite the following file? <br /><b>\"%1\"</b></p>")
                            .arg(filename),
                            QMessageBox::Yes | QMessageBox::Cancel,
                            QMessageBox::Cancel);
}

int Alia::queryBeforeEcFileImport(const QString& filename)
{
    return QMessageBox::question(0,
                tr("Import Project"),
                tr("Your project file has to be imported "
                   "and updated to a new version. "
                   "If you proceed, you will "
                   "lose your file and the "
                   "compatibility with previous versions of EddyPro "
                   "but you will have a smooth "
                   "transition to the new EddyPro version. "
                   "If you are unsure, "
                   "select cancel and create a backup copy of your "
                   "project file before proceeding. "
                   "<p>Are you sure you want to "
                   "import the following file? <br />"
                   "<b>\"%1\"</b></p>")
                .arg(filename),
                QMessageBox::Yes | QMessageBox::Cancel,
                QMessageBox::Cancel);
}

int Alia::infoBeforeMdFileImport(const QString& filename)
{
    Q_UNUSED(filename)

    return QMessageBox::information(0,
                tr("Import Metadata"),
                tr("Your metadata file must be imported "
                   "to a new version for compatibility. "
                   "Save the metadata file with a new file name "
                   "to create a backup copy "
                   "or simply overwrite it."),
                QMessageBox::Ok);
}

int Alia::noConnectionMsg()
{
    return QMessageBox::warning(0,
                            tr("Connection Problem"),
                            tr("<p>No connection available or connection error updating the magnetic declination.</p>"),
                            QMessageBox::Ok);
}

int Alia::noNoaaDownloadMsg()
{
    bool showDialog = false;

    QSettings config;
    config.beginGroup(Defs::CONFGROUP_WINDOW);
        showDialog = config.value(Defs::CONF_WIN_NOAA_WEBSITE_MSG, true).toBool();
    config.endGroup();

    if (showDialog)
    {
        // info message
        InfoMessage noaaDialog(QDialogButtonBox::Ok, 0);
        noaaDialog.setTitle(tr("NOAA Download Problem"));
        noaaDialog.setType(InfoMessage::NOAA_WEBSITE);
        noaaDialog.setMessage(tr("<p>Server not responding or service not available "
                                            "updating the magnetic declination.</p>"));
        noaaDialog.refresh();
        return noaaDialog.exec();
    }
    return QMessageBox::Ok;
}

void Alia::updateLastDatapath(const QString& dir)
{
    if (!dir.isEmpty())
    {
        QSettings settings;
        settings.beginGroup(Defs::CONFGROUP_WINDOW);
            settings.setValue(Defs::CONF_WIN_LAST_DATAPATH, dir);
        settings.endGroup();
        settings.sync();
    }
}

bool Alia::isGoodRawFileNameFormat(const QString& s)
{
    DEBUG_FUNC_NAME
    bool test = !s.isEmpty()
                && s.contains(QStringLiteral("yy"))
                && s.contains(QStringLiteral("dd"))
                && s.contains(QStringLiteral("HH"))
                && s.contains(QStringLiteral("MM"))
                && s.contains(QStringLiteral("."));
    qDebug() << "test" << test;
    return test;
}

const QColor Alia::getColor(int step)
{
    QColor c("#aae6ff");
    int h = c.hue();
    int s = c.saturation();
    int v = c.value();

    c.setHsv(abs((h - (step * 10))) % 360, s, v);

    return c;
}

bool Alia::isGoodGasUnit(const QString& unit, const QString& type)
{
    bool isGoodUnit = false;
    if (!unit.isEmpty())
    {
        if (type == VariableDesc::getVARIABLE_MEASURE_TYPE_STRING_0())
        {
            isGoodUnit = (unit == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_12())
                               || (unit == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_13())
                               || (unit == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_19())
                               || (unit == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_20())
                               || (unit == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_21())
                               || (unit == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_18());
        }
        else if ((type == VariableDesc::getVARIABLE_MEASURE_TYPE_STRING_1())
                 || (type == VariableDesc::getVARIABLE_MEASURE_TYPE_STRING_2()))
        {
            isGoodUnit = (unit == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_9())
                               || (unit == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_10())
                               || (unit == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_11());
        }
    }
    return isGoodUnit;
}

// NOTE: hack useful to trigger tooltip updates
void Alia::resetComboToItem(QComboBox *combo, int i)
{
    DEBUG_FUNC_NAME
    combo->setCurrentIndex(-1);
    combo->setCurrentIndex(i);
}

void Alia::updateComboItemTooltip(QComboBox *combo, int i)
{
    combo->setToolTip(combo->itemData(i, Qt::ToolTipRole).toString());
}

void Alia::updateLineEditToolip(QLineEdit *lineedit)
{
    lineedit->setToolTip(QString(QStringLiteral("%1")).arg(lineedit->text()));
}

// http://stackoverflow.com/questions/2404449/process-starturl-with-anchor-in-the-url
bool Alia::launchWebBrowser(const QUrl &url)
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
            QSettings settings_3(QStringLiteral("HKCU\\Software\\Microsoft\\Windows\\Shell\\Associations\\UrlAssociations\\http\\UserChoice"),
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

const QVariant Alia::currentItemData(QComboBox* combo)
{
    return combo->itemData(combo->currentIndex());
}

void Alia::openWebSite()
{
    QDesktopServices::openUrl(QUrl(QStringLiteral("http://www.licor.com/env/products/eddy_covariance/software.html")));
}

void Alia::setTextToLabel(QLabel *label, const QString& text, Qt::TextElideMode mode, int width)
{
    QFontMetrics fm(label->font());
    int realWidth = width - 40;
    QString clippedText = fm.elidedText(text, mode, realWidth);
    label->setText(clippedText);
}

void Alia::setTextToLineEdit(QLineEdit *lineEdit, const QString& text, Qt::TextElideMode mode, int width)
{
    DEBUG_FUNC_NAME
    qDebug() << "width" << width;
    QFontMetrics fm(lineEdit->font());
    int realWidth = width - 40;
    QString clippedText = fm.elidedText(text, mode, realWidth);
    lineEdit->setText(clippedText);
}

// NOTE: hack to show the calendar,
// because dateEdit->calendarWidget()->show() seems not working
void Alia::showCalendarOf(QWidget *widget)
{
    QCoreApplication::postEvent(widget,
                                new QMouseEvent(QEvent::MouseButtonPress,
                                                QPoint(90, 5),
                                                Qt::LeftButton,
                                                Qt::NoButton,
                                                Qt::NoModifier));
}

void  Alia::getCustomVariables(QStringList* varList)
{

    QSettings settings;
    settings.beginGroup(Defs::CONFGROUP_PROJECT);
    QString stringValue = settings.value(Defs::CONF_PROJ_CUSTOM_VARS, QString()).toString();
    settings.endGroup();

    *varList = stringValue.split(QLatin1Char(','), QString::SkipEmptyParts);
}

void Alia::setCustomVariables(const QStringList& varList)
{
    if (!varList.isEmpty())
    {
        QSettings settings;
        settings.beginGroup(Defs::CONFGROUP_PROJECT);
        settings.setValue(Defs::CONF_PROJ_CUSTOM_VARS, varList.join(QStringLiteral(",")));
        settings.endGroup();
        settings.sync();
    }
}

void Alia::information(QWidget *parent, const QString &title,
                 const QString &text, const QString &detailedText)
{
    QScopedPointer<QMessageBox> messageBox(new QMessageBox(parent));
    if (parent)
    {
        messageBox->setWindowModality(Qt::WindowModal);
    }
    messageBox->setWindowTitle(QString(QStringLiteral("%1 - %2"))
            .arg(QApplication::applicationName()).arg(title));
    messageBox->setText(text);
    if (!detailedText.isEmpty())
        messageBox->setInformativeText(detailedText);
    messageBox->setIcon(QMessageBox::Information);
    messageBox->addButton(QMessageBox::Ok);
    messageBox->exec();
}


void Alia::warning(QWidget *parent, const QString &title,
             const QString &text, const QString &detailedText)
{
    QScopedPointer<QMessageBox> messageBox(new QMessageBox(parent));
    if (parent)
        messageBox->setWindowModality(Qt::WindowModal);
    messageBox->setWindowTitle(QString(QStringLiteral("%1 - %2"))
            .arg(QApplication::applicationName()).arg(title));
    messageBox->setText(text);
    if (!detailedText.isEmpty())
        messageBox->setInformativeText(detailedText);
    messageBox->setIcon(QMessageBox::Warning);
    messageBox->addButton(QMessageBox::Ok);
    messageBox->exec();
}


bool Alia::question(QWidget *parent, const QString &title,
              const QString &text, const QString &detailedText,
              const QString &yesText, const QString &noText)
{
    QScopedPointer<QMessageBox> messageBox(new QMessageBox(parent));
    if (parent)
        messageBox->setWindowModality(Qt::WindowModal);
    messageBox->setWindowTitle(QString(QStringLiteral("%1 - %2"))
            .arg(QApplication::applicationName()).arg(title));
    messageBox->setText(text);
    if (!detailedText.isEmpty())
        messageBox->setInformativeText(detailedText);
    messageBox->setIcon(QMessageBox::Question);
    QPushButton *yesButton = messageBox->addButton(yesText,
            QMessageBox::AcceptRole);
    messageBox->addButton(noText, QMessageBox::RejectRole);
    messageBox->setDefaultButton(yesButton);
    messageBox->exec();
    return messageBox->clickedButton() == yesButton;
}
