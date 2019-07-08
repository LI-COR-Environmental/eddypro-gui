/***************************************************************************
  advsettingscontainer.h
  -------------------
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

#ifndef ADVSETTINGSCONTAINER_H
#define ADVSETTINGSCONTAINER_H

#include <QWidget>

#include "configstate.h"

class QStackedLayout;

class AdvOutputOptions;
class AdvProcessingOptions;
class AdvSpectralOptions;
class AdvStatisticalOptions;
class DlProject;
class EcProject;

class AdvSettingsContainer : public QWidget
{
    Q_OBJECT

public:
    AdvSettingsContainer(QWidget* parent,
                         DlProject* dlProject,
                         EcProject* ecProject,
                         ConfigState* config);

    AdvProcessingOptions *processingOptions() { return processingOptions_; }
    AdvSpectralOptions *spectralOptions() { return spectralOptions_; }
    AdvStatisticalOptions *statisticalOptions() { return statisticalOptions_; }
    AdvOutputOptions *outputOptions() { return outputOptions_; }

signals:
    void checkMetadataOutputRequest();

public slots:
    void setCurrentPage(int page);

private:
    AdvProcessingOptions* processingOptions_ {};
    AdvStatisticalOptions* statisticalOptions_ {};
    AdvOutputOptions* outputOptions_ {};
    AdvSpectralOptions* spectralOptions_ {};
    QStackedLayout* mainLayout_ {};

    DlProject* dlProject_;
    EcProject* ecProject_;
    ConfigState* configState_;
};

#endif // ADVSETTINGSCONTAINER_H
