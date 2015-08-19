/***************************************************************************
  biometmetadatareader.cpp
  -------------------
  Copyright (C) 2013-2014, LI-COR Biosciences
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

#include "biommetadatareader.h"

#include <QDebug>
#include <QFile>
#include <QSettings>

#include "bminidefs.h"
#include "dbghelper.h"

const QString BiomMetadataReader::getVAR_TA()
{
    static const QString s(QStringLiteral("TA"));
    return s;
}

const QString BiomMetadataReader::getVAR_PA()
{
    static const QString s(QStringLiteral("PA"));
    return s;
}

const QString BiomMetadataReader::getVAR_RH()
{
    static const QString s(QStringLiteral("RH"));
    return s;
}

const QString BiomMetadataReader::getVAR_RG()
{
    static const QString s(QStringLiteral("RG"));
    return s;
}

const QString BiomMetadataReader::getVAR_LWIN()
{
    static const QString s(QStringLiteral("LWIN"));
    return s;
}

const QString BiomMetadataReader::getVAR_PPFD()
{
    static const QString s(QStringLiteral("PPFD"));
    return s;
}

BiomMetadataReader::BiomMetadataReader(QList<BiomItem> *biomMetadata)
    : biomMetadata_(biomMetadata)
{
}

bool BiomMetadataReader::readEmbMetadata(const QString& fileName)
{
    DEBUG_FUNC_NAME

    // open file
    QFile dataFile(fileName);
    if (!dataFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        // error opening file
        qDebug() << "Error: Cannot open" << fileName << "file";
        return false;
    }

    // read file
    QSettings settings(fileName, QSettings::IniFormat);

    // try old format
    settings.beginGroup(BmIni::INIGROUP_VARS_OLD);

    // iterate through instrument list
    int numVars = countEmbVariables(settings.allKeys());
    qDebug() << "numVars:" << numVars;

    if (numVars == 0)
    {
        settings.endGroup();

        // try new format
        settings.beginGroup(BmIni::INIGROUP_VARS);
        numVars = countEmbVariables(settings.allKeys());
        qDebug() << "numVars:" << numVars;
    }

    for (int k = 0; k < numVars; ++k)
    {
        QString prefix = BmIni::INI_PREFIX + QString::number(k + 1) + QStringLiteral("_");
        QString var = settings.value(prefix + BmIni::INI_VARS_0).toString();
        QString id = settings.value(prefix + BmIni::INI_VARS_1).toString();

        // skip entries with no type defined
        if (!var.isEmpty())
        {
            QStringList allowedVars;
            allowedVars << getVAR_TA()
                        << getVAR_PA()
                        << getVAR_RH()
                        << getVAR_RG()
                        << getVAR_LWIN()
                        << getVAR_PPFD();
            if (!allowedVars.filter(var).isEmpty())
            {
                biomMetadata_->append(BiomItem(var, id, k + 1));
            }
        }
    }
    settings.endGroup();
    dataFile.close();

    return true;
}

int BiomMetadataReader::countEmbVariables(const QStringList& list)
{
    int i = 0;
    foreach (const QString& s, list)
    {
        if (s.contains(BmIni::INI_VARS_0))
            ++i;
    }
    return i;
}

bool BiomMetadataReader::readAltMetadata(const QString& fileName)
{
    DEBUG_FUNC_NAME

    // open file
    QFile dataFile(fileName);
    if (!dataFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        // error opening file
        qDebug() << "Error: Cannot open" << fileName << "file";
        return false;
    }

    // read file
    QByteArray baLine = dataFile.readLine();
    const char* lineContent = baLine.constData();
    QString line = QString::fromUtf8(lineContent);

    if (!line.isEmpty())
    {
        // skip first 2 rows
        if (line.contains(QStringLiteral("Station Name")))
        {
            qDebug() << "skip first row";

            // read a line
            baLine = dataFile.readLine();
            lineContent = baLine.constData();
            line = QString::fromUtf8(lineContent);

            if (line.contains(QStringLiteral("UC4")))
            {
                qDebug() << "skip second row";

                // read a line
                baLine = dataFile.readLine();
                lineContent = baLine.constData();
                line = QString::fromUtf8(lineContent);
            }
        }

        QStringList strings = line.split(QLatin1Char(','));

        // iterate ont the vars list
        for (int k = 0; k < strings.count(); ++k)
        {
            QStringList varStrList = strings.at(k).split(QLatin1Char('_'));
            QString id = varStrList.at(0).toUpper();

            qDebug() << "k" << k << "id" << id;

            QStringList allowedVars;
            allowedVars << getVAR_TA()
                        << getVAR_PA()
                        << getVAR_RH()
                        << getVAR_RG()
                        << getVAR_LWIN()
                        << getVAR_PPFD();
            if (!allowedVars.filter(id).isEmpty())
            {
                biomMetadata_->append(BiomItem(id, id, k + 1));
            }
        }
    }
    dataFile.close();

    return true;
}
