/***************************************************************************
  dlsitetab.h
  -------------------
  Copyright (C) 2007-2011, Eco2s team, Antonio Forgione
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
    virtual ~DlSiteTab();

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

    QPushButton *questionMark_1;
    QPushButton *questionMark_2;
    QPushButton *questionMark_3;

    void createQuestionMark();

    DlProject *project_;
};

#endif // DLSITETAB_H
