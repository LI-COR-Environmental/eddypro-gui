/***************************************************************************
  biometmetadatareader.cpp
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

    // try old format first
    settings.beginGroup(BmIni::INIGROUP_VARS_OLD);

    // iterate through instrument list
    auto numVars = countEmbVariables(settings.allKeys());

    // if no variables are found, try new format
    if (numVars == 0)
    {
        settings.endGroup();

        settings.beginGroup(BmIni::INIGROUP_VARS);
        numVars = countEmbVariables(settings.allKeys());
    }

    for (auto k = 0; k < numVars; ++k)
    {
        auto prefix = BmIni::INI_PREFIX;
        prefix += QString::number(k + 1);
        prefix += QStringLiteral("_");

        auto var = settings.value(prefix + BmIni::INI_VARS_0, QString()).toString();

        // NOTE: not really needed for now
        auto id = settings.value(prefix + BmIni::INI_VARS_1, QString()).toString();

        // skip entries with no type ('variable' field in the biomet metadata
        // file) defined
        if (var.isEmpty())
        {
            continue;
        }

        // variables types the GUI is allowed to show and manage for now
        QStringList allowedVarIDs;
        allowedVarIDs << getVAR_TA()
                      << getVAR_PA()
                      << getVAR_RH()
                      << getVAR_RG()
                      << getVAR_LWIN()
                      << getVAR_PPFD();

        // get components of the variable type field
        // it can be simple or with positional notation defined
        auto type_components_list = var.split(QLatin1Char('_'));

        auto extracted_type = QString();
        // entry with positional notation and underscore(s)
        // in the variable name (e.g. P_RAIN_1_1_1)
        auto type_components_size = type_components_list.size();
        if (type_components_size > 4)
        {
            QStringList extracted_var_name = type_components_list.mid(0, type_components_size - 3);
            extracted_type = extracted_var_name.join(QStringLiteral("_"));
        }
        // entry with positional notation and no underscore
        // in the variable name (e.g. PA_1_1_1)
        // or entry with no positional notation (e.g. DATE)
        else
        {
            extracted_type = type_components_list.first();
        }

        auto allowedVar = allowedVarIDs.filter(extracted_type);
        // skip not allowed entries
        if (allowedVar.isEmpty())
        {
            continue;
        }

        // add allowed biogeo variables
        biomMetadata_->append(BiomItem(var, id, k + 1));
    }
    settings.endGroup();
    dataFile.close();

    return true;
}

int BiomMetadataReader::countEmbVariables(const QStringList& list)
{
    auto i = 0;
    foreach (const QString& s, list)
    {
        if (s.contains(BmIni::INI_VARS_0))
        {
            ++i;
        }
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
    auto baLine = dataFile.readLine();
    auto lineContent = baLine.constData();
    auto line = QString::fromUtf8(lineContent);

    if (!line.isEmpty())
    {
        // skip first 2 rows
        if (line.contains(QStringLiteral("Station Name")))
        {
            // read a line
            baLine = dataFile.readLine();
            lineContent = baLine.constData();
            line = QString::fromUtf8(lineContent);

            if (line.contains(QStringLiteral("UC4")))
            {
                // read a line
                baLine = dataFile.readLine();
                lineContent = baLine.constData();
                line = QString::fromUtf8(lineContent);
            }
        }

        auto strings = line.split(QLatin1Char(','));

        // iterate on the variable list
        for (auto k = 0; k < strings.count(); ++k)
        {
            auto var = strings.at(k).split(QLatin1Char('_'));
            auto id = var.at(0).toUpper().trimmed();

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
