/***************************************************************************
  lineeditandbrowsewidget.h
  -------------------
  Widget with combination of line edit and browse button
  -------------------
  Copyright (C) 2016-2018, LI-COR Biosciences
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

#ifndef LINEEDITANDBROWSEWIDGET_H
#define LINEEDITANDBROWSEWIDGET_H

#include <QWidget>

class CustomDropLineEdit;
class QPushButton;

class LineEditAndBrowseWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LineEditAndBrowseWidget(QWidget *parent = nullptr);

    QString toolTip() const;
    void setToolTip(const QString &text);

    QString buttonText() const;
    void setButtonText(const QString &text);

    QString text() const;
    void setText(const QString &text);

    QString path() const;
    void setPath(const QString &path);

    QString dialogTitle() const { return dialogTitle_; }
    void setDialogTitle(const QString &title) { dialogTitle_ = title; }

    QString dialogWorkingDir() const { return dialogDir_; }
    void setDialogWorkingDir(const QString &dir) { dialogDir_ = dir; }

    void focusAndSelect() const;

    ///
    /// \brief disableClearAction
    /// Disable automatic clear on the lineedit.
    /// Useful for providing custom behaviors.
    void disableClearAction() const;

    int returnLineEditWidth() const;

    QPushButton *button() const;

    void disableClickAction() const;

signals:
    ///
    /// \brief clearRequested
    /// Sent to notify clear button toggling.
    void clearRequested();

    ///
    /// \brief pathChanged
    /// Sent in case of programmatic line edit changes.
    void pathChanged(const QString&);

    ///
    /// \brief pathSelected
    /// Sent in case of successful user interaction.
    void pathSelected(const QString&);

protected:
    CustomDropLineEdit *lineEdit() const;

public slots:
    void clear();
    void setEnabled(bool enable);

private slots:
    void updatePathTooltip();
    virtual void onButtonClick() = 0;

private:
    CustomDropLineEdit *lineEdit_;
    QPushButton *button_;
    QString dialogTitle_;
    QString dialogDir_;
};

#endif  // LINEEDITANDBROWSEWIDGET_H
