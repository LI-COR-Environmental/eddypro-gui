/***************************************************************************
  biometmetadatareader.h
  ----------------------
  Copyright © 2013-2019, LI-COR Biosciences, Inc. All Rights Reserved.
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

#ifndef BIOMMETADATAREADER_H
#define BIOMMETADATAREADER_H

#include <QList>
#include <QStringList>

#include "biomitem.h"

class BiomMetadataReader
{
public:
    explicit BiomMetadataReader(QList<BiomItem>* biomMetadata);

    bool readEmbMetadata(const QString& fileName);
    bool readAltMetadata(const QString& fileName);

    static const QString getVAR_TA();
    static const QString getVAR_PA();
    static const QString getVAR_RH();
    static const QString getVAR_RG();
    static const QString getVAR_LWIN();
    static const QString getVAR_PPFD();

private:
    int countEmbVariables(const QStringList& list);

    QList<BiomItem>* biomMetadata_;
};

#endif // BIOMMETADATAREADER_H
