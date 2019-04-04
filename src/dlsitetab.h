/***************************************************************************
  dlsitetab.h
  -----------
  Copyright © 2007-2011, Eco2s team, Antonio Forgione
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
 ***************************************************************************/

#ifndef DLSITETAB_H
#define DLSITETAB_H

#include <QWidget>

class QDoubleSpinBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QSpinBox;

class ClickLabel;
class DlProject;

class DlSiteTab : public QWidget {
    Q_OBJECT

public:
    DlSiteTab(QWidget *parent, DlProject *dlProject);

    void reset();
    void refresh();

    QDoubleSpinBox *altitudeSpin;
    QDoubleSpinBox *canopyHeightSpin;
    QDoubleSpinBox *displacementHeightSpin;
    QDoubleSpinBox *roughnessLengthSpin;

    QLineEdit *latitudeEdit;
    QRadioButton *sexDegRadio;

public slots:
    QString strLatitude(double lat);

private slots:
    double numLatitude(const QString &text);
    double numLongitude(const QString &text);
    QString strLongitude(double lon);
    void sexDegRadioToogled(bool checked);
    void decDegRadioToogled(bool checked);

    void updateAltitude(double n);
    void updateLatitude(const QString & lat);
    void updateLongitude(const QString & lon);
    void updateCanopyHeight(double d);
    void updateDisplacementHeight(double d);
    void updateRoughnessLength(double d);
    void updateAcquisitionFrequency(double d);

    void onClickAltitudeLabel();
    void onClickCanopyLabel();
    void onClickDisplacementLabel();
    void onClickRoughnessLabel();
    void onClickLatitudeLabel();
    void onClickLongitudeLabel();
    void onClickFreqLabel();

    void updateTimestampRef(int n);
    void updateFileDuration(int n);
    void onClickDurationLabel();

    void onlineHelpTrigger_1();
    void onlineHelpTrigger_2();
    void onlineHelpTrigger_3();

private:
    ClickLabel *altitudeLabel;
    ClickLabel *canopyLabel;
    ClickLabel *displacementLabel;
    ClickLabel *roughnessLabel;

    ClickLabel *timestampRefLabel;
    QRadioButton *timestampRefStartRadio;
    QRadioButton *timestampRefEndRadio;
    ClickLabel *durationLabel;
    QSpinBox *fileDurationSpin;
    ClickLabel *freqLabel;
    QDoubleSpinBox *acquisitionFreqSpin;

    ClickLabel *latitudeLabel;
    ClickLabel *longitudeLabel;
    QRadioButton *decDegRadio;
    QLineEdit *longitudeEdit;
    bool decimalDegrees_;

    QPushButton *questionMark_1 = nullptr;
    QPushButton *questionMark_2 = nullptr;
    QPushButton *questionMark_3 = nullptr;

    void createQuestionMark();

    DlProject *project_;
};

#endif // DLSITETAB_H
