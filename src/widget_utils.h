/***************************************************************************
  widget_utils.h
  -------------------
  Copyright (C) 2014-2016, LI-COR Biosciences
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
class QScrollArea;
class QTextEdit;
class QWidget;

class QScienceSpinBox;

struct ConfigState;

namespace WidgetUtils
{
    using PropertyList = QPair<const char*, bool>;

    // color
    const QColor getColor(int step);

    // stylesheet
    void updatePropertyListAndStyle(QWidget* widget,
                                    QList<PropertyList> propertyList);

    // combo
    QVariant currentComboItemData(QComboBox* combo, int role = Qt::UserRole);
    void resetComboToItem(QComboBox* combo, int i);
    void updateComboItemTooltip(QComboBox* combo, int i);

    // buttons
    QPushButton* createCommonButton(QWidget *parent, const QString &text);

    // dialog
    void removeContextHelpButton(QWidget* w);

    // label
    QLabel* createBlueLabel(QWidget* parent, const QString &text);
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

    // spinbox
    QScienceSpinBox* createCalibrationSpinbox(QWidget *parent);

    // calendar
    void customizeCalendar(QCalendarWidget* cal);
    void showCalendarOf(QWidget* widget);

    // textedit
    void appendHrToTextEdit(QTextEdit* te);

    // browser launcher
    void openAppWebsite();
    void showHelp(const QUrl& url);

    // message box
    bool okToOverwrite(QWidget *parent, const QString& filename);
    bool okToRemoveColumn(QWidget* parent);
    bool okToCloseSmartFlux(QWidget* parent);
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
                 const QString& infoText = QString(),
                 const QString& objectName = QString());
    void critical(QWidget* parent,
                  const QString& title,
                  const QString& text,
                  const QString& infoText = QString());
    bool yesNoQuestion(QWidget* parent,
                  const QString& title,
                  const QString& text,
                  const QString& infoText = QString(),
                  const QString& objectName = QString(),
                  const QString& yesText = QObject::tr("&Yes"),
                  const QString& noText = QObject::tr("&No"));

    // progress bar
    void setProgressValue(QProgressBar* bar, int value);

    // make container scroll area
    QScrollArea *getContainerScrollArea(QWidget* parent, QLayout *layout);

    // provide a search path hint
    QString getSearchPathHint();

}  // namespace WidgetUtils

#endif  // WIDGET_UTILS_H
