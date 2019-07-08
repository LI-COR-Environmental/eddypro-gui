/***************************************************************************
  richtextcheckbox.h
  ------------------
  Copyright © 2011-2019, LI-COR Biosciences, Inc. All Rights Reserved.
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
    explicit RichTextCheckBox(QWidget *parent = nullptr);

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
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

private slots:
    void triggerOnlineHelp();

private:
    HoverCheckBox* checkBox;
    ClickableLabel* label;
    QPushButton* questionMark;
    QUrl questionMarkLink;
};

#endif  // RICHTEXTCHECKBOX_H
