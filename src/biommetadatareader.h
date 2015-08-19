/***************************************************************************
  biometmetadatareader.h
  -------------------
  Copyright (C) 2013-2015, LI-COR Biosciences
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
