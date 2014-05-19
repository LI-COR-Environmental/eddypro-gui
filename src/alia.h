/***************************************************************************
  alia.h
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

#ifndef ALIA_H
#define ALIA_H

#include <QtGlobal>
#include <QVariant>
#include <QCoreApplication>

class QString;
class QCheckBox;
class QGlobalColor;
class QStringList;
class QUrl;
class QCalendarWidget;
class QComboBox;
class QLabel;
class QLineEdit;

class Alia
{
    Q_DECLARE_TR_FUNCTIONS(Alia)

public:
    typedef QPair<const char*, bool> PropertyList;

    static void updateStyle(QWidget* widget);
    static void updatePropertyAndStyle(QWidget* widget, const char * name, const QVariant & value);
    static void updatePropertyListAndStyle(QWidget* widget, QList<PropertyList> propertyList);

    static bool isOnline();
    static bool isOnline_2();
    static void showHelp(const QUrl& url);

    static void customizeCalendar(QCalendarWidget* cal);

    static int queryEcReset_1();
    static int queryEcReset_2();
    static int queryEcReset_3();
    static int queryMdReset();
    static int queryOverwrite(const QString& filename);
    static int queryBeforeEcFileImport(const QString& filename);
    static int infoBeforeMdFileImport(const QString &filename);

    static int noConnectionMsg();
    static int noNoaaDownloadMsg();

    static void updateLastDatapath(const QString& dir);

    static bool isGoodRawFileNameFormat(const QString& s);

    static const QColor getColor(int step);

    static bool isGoodGasUnit(const QString &unit, const QString &type);

    static void resetComboToItem(QComboBox *combo, int i);

    static void updateComboItemTooltip(QComboBox *combo, int i);

    static void updateLineEditToolip(QLineEdit* lineedit);

    static bool launchWebBrowser(const QUrl &url);

    static const QVariant currentItemData(QComboBox* combo);

    static void openWebSite();

    static void setTextToLabel(QLabel *label, const QString &text, Qt::TextElideMode mode, int width);

    static void setTextToLineEdit(QLineEdit *lineEdit, const QString &text, Qt::TextElideMode mode, int width);

    static void showCalendarOf(QWidget* widget);

    static void getCustomVariables(QStringList *varList);
    static void setCustomVariables(const QStringList &varList);

    // from AQP, http://www.qtrac.eu/aqpbook.html, under GPL3
    /*
        Copyright (c) 2009-10 Qtrac Ltd. All rights reserved.

        This program or module is free software: you can redistribute it
        and/or modify it under the terms of the GNU General Public License
        as published by the Free Software Foundation, either version 3 of
        the License, or (at your option) any later version. It is provided
        for educational purposes and is distributed in the hope that it will
        be useful, but WITHOUT ANY WARRANTY; without even the implied
        warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
        the GNU General Public License for more details.
    */
    static void information(QWidget *parent, const QString &title,
            const QString &text, const QString &detailedText = QString());
    static void warning(QWidget *parent, const QString &title,
            const QString &text, const QString &detailedText = QString());
    static bool question(QWidget *parent, const QString &title,
            const QString &text, const QString &detailedText = QString(),
            const QString &yesText = QObject::tr("&Yes"),
            const QString &noText = QObject::tr("&No"));
};

#endif // ALIA_H
