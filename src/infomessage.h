/***************************************************************************
  infomessages.h
  --------------
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

#ifndef INFOMESSAGE_H
#define INFOMESSAGE_H

#include <QDialog>
#include <QDialogButtonBox>

class QCheckBox;
class QLabel;

class InfoMessage : public QDialog
{
    Q_OBJECT

public:
    enum class Type
    {
        RUN_EXPRESS,
        RUN_ADVANCED,
        RUN_RETRIEVER,
        SELECTION_CLEANING,
        SMARTFLUX_CONFIG,
        NOAA_WEBSITE,
        ANGLE_OF_ATTACK_SELECTION
    };

    InfoMessage(QDialogButtonBox::StandardButtons buttons,
                QWidget* parent = nullptr);

    void setTitle(const QString& title);
    void setIcon(const QPixmap &icon);
    void setMessage(const QString& text);
    void setType(Type type);
    void setDoNotShowAgainVisible(bool visible);
    void refresh();

private slots:
    void initialize();
    void onDoNotShowAgainCheckboxToggled_(bool toggled);
    void onOkButtonClicked();
    void onYesButtonClicked();
    void onNoButtonClicked();
    void onCancelButtonClicked();

private:
    QString title_{};
    QLabel* icon_;
    QLabel* message_;
    QCheckBox* doNotShowAgainCheckbox_{};
    QDialogButtonBox* buttonBox_{};
    Type type_{Type::RUN_EXPRESS};
    bool doNotShoAgainVisible_{true};
};

#endif // INFOMESSAGE_H
