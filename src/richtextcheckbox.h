/***************************************************************************
  richtextcheckbox.h
  -------------------
  Checkbox with rich text support
  -------------------
  Copyright (C) 2011-2018, LI-COR Biosciences
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

#ifndef RICHTEXTCHECKBOX_H
#define RICHTEXTCHECKBOX_H

#include <QWidget>
#include <QUrl>

class QPushButton;

class ClickableLabel;
class HoverCheckBox;

class RichTextCheckBox : public QWidget
{
    Q_OBJECT
public:
    explicit RichTextCheckBox(QWidget *parent = 0);
    ~RichTextCheckBox();

    HoverCheckBox* getCheckBox() { return checkBox; }

    QString text() const;
    void setText(const QString &text);

    QString plainText() const;

    Qt::CheckState checkState() const;
    bool isChecked() const;

    void clearStates();

    void setQuestionMark(const QString& url);

signals:
    void toggled(bool on);
    void clicked();

public slots:
    void setChecked(bool checked);
    void setCheckState(Qt::CheckState state);

protected:
    void enterEvent(QEvent *event) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *event) Q_DECL_OVERRIDE;

private slots:
    void triggerOnlineHelp();

private:
    HoverCheckBox* checkBox;
    ClickableLabel* label;
    QPushButton* questionMark;
    QUrl questionMarkLink;
};

#endif  // RICHTEXTCHECKBOX_H
