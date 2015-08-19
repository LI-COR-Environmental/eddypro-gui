/***************************************************************************
  widget_utils.h
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

#ifndef WIDGET_UTILS_H
#define WIDGET_UTILS_H

#include <QColor>
#include <QMessageBox>
#include <QPair>

class QCalendarWidget;
class QComboBox;
class QLabel;
class QLineEdit;
class QProgressBar;
class QTextEdit;
class QWidget;

namespace WidgetUtils
{
    typedef QPair<const char*, bool> PropertyList;

    // color
    const QColor getColor(int step);

    // stylesheet
    void updatePropertyListAndStyle(QWidget* widget,
                                    QList<PropertyList> propertyList);

    // combo
    QVariant currentComboItemData(QComboBox* combo, int role = Qt::UserRole);
    void resetComboToItem(QComboBox* combo, int i);
    void updateComboItemTooltip(QComboBox* combo, int i);

    // dialog
    void removeContextHelpButton(QWidget* w);

    // label
    void setElidedTextToLabel(QLabel* label,
                              const QString& text,
                              Qt::TextElideMode mode,
                              int width);

    // lineedit
    void setElidedTextToLineEdit(QLineEdit* lineEdit,
                                 const QString& text,
                                 Qt::TextElideMode mode,
                                 int width);
    void updateLineEditToolip(QLineEdit* lineedit);

    // calendar
    void customizeCalendar(QCalendarWidget* cal);
    void showCalendarOf(QWidget* widget);

    // textedit
    void appendHrToTextEdit(QTextEdit* te);

    // browser launcher
    void openAppWebsite();
    void showHelp(const QUrl& url);

    // message box
    bool okToOverwrite(const QString& filename);
    bool okToRemoveColumn(QWidget* parent);
    QMessageBox::ButtonRole requestToSave(QWidget* parent,
                                          const QString& title,
                                          const QString& text,
                                          const QString& infoText);

    // inspired by the AQP book, http://www.qtrac.eu/aqpbook.html, under GPL3
    bool information(QWidget* parent,
                     const QString& title,
                     const QString& text,
                     const QString& infoText = QString());
    void warning(QWidget* parent,
                 const QString& title,
                 const QString& text,
                 const QString& infoText = QString());
    void critical(QWidget* parent,
                  const QString& title,
                  const QString& text,
                  const QString& infoText = QString());
    bool okToQuestion(QWidget* parent,
                  const QString& title,
                  const QString& text,
                  const QString& infoText = QString(),
                  const QString& yesText = QObject::tr("&Yes"),
                  const QString& noText = QObject::tr("&No"));

    // progress bar
    void setProgressValue(QProgressBar* bar, int value);

}  // namespace WidgetUtils

#endif  // WIDGET_UTILS_H
