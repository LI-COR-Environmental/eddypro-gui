/***************************************************************************
  lineeditandbrowsewidget.h
  -------------------------
  Copyright © 2016-2019, LI-COR Biosciences, Inc. All Rights Reserved.
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
