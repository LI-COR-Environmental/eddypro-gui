/***************************************************************************
  widget_utils.h
  --------------
  Copyright © 2014-2019, LI-COR Biosciences, Inc. All Rights Reserved.
  Author: Antonio Forgione

  This file is part of EddyPro®.

  NON-COMMERCIAL RESEARCH PURPOSES ONLY - EDDYPRO® is licensed for
  non-commercial academic and government research purposes only,
  as provided in the EDDYPRO® End User License Agreement.
  EDDYPRO® may only be used as provided in the End User License Agreement
  and may not be used or accessed for any commercial purposes.
  You may view a copy of the End User License Agreement in the file
  EULA_NON_COMMERCIAL.rtf.

  Commercial companies that are LI-COR flux system customers are
  encouraged to contact LI-COR directly for our commercial EDDYPRO®
  End User License Agreement.

  EDDYPRO® contains Open Source Components (as defined in the
  End User License Agreement). The licenses and/or notices for the
  Open Source Components can be found in the file LIBRARIES.txt.

  EddyPro® is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
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
    void removeFlagFromWidget(Qt::WindowFlags flag, QWidget* w);
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

    // calendar
    void customizeCalendar(QCalendarWidget* cal);
    void showCalendarOf(QWidget* widget);

    // textedit
    void appendHrToTextEdit(QTextEdit* te);

    // browser launcher
    void openAppWebsite();
    void openForumWebsite();
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
