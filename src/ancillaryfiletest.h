/***************************************************************************
  ancillaryfiletest.h
  -------------------
  Copyright (C) 2014, LI-COR Biosciences
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

#ifndef ANCILLARYFILETEST_H
#define ANCILLARYFILETEST_H

#include <QList>
#include <QDialog>
#include <QMap>
#include <QStringList>

# include "defs.h"

class QTextBrowser;

class AncillaryFileTest : public QDialog
{
    Q_OBJECT
public:
    enum class FileType { Spectra, PlanarFit, TimeLag };

    explicit AncillaryFileTest(FileType type, QWidget* parent = nullptr);

    bool makeTest();
    void refresh(const QString &file);

private:
    typedef QList<QStringList> LineList;

    struct FileTemplate {
        QString filepath;
        bool (AncillaryFileTest::*formalTest)(const LineList &, const LineList &);
        bool (AncillaryFileTest::*scientificTest)(const LineList &);
    };

    QString formatPassFail(bool test_result);

    bool testFile();
    bool parseFile(const QString &filename, LineList *lines);

    bool testSpectraF(const LineList &templateList, const LineList &actualList);
    bool testSpectraS(const LineList &actualList);

    bool testPlanarFitF(const LineList &templateList, const LineList &actualList);
    bool testPlanarFitS(const LineList &actualList);

    bool testTimeLagF(const LineList &templateList, const LineList &actualList);
    bool testTimeLagS(const LineList &actualList);

    FileType type_;

    QString name_ {};
    QTextBrowser* testResults_ {};

    // NOTE: might use something else, for example std::unordered_map
    QMap<FileType, AncillaryFileTest::FileTemplate> testFileMap_ {
        { FileType::Spectra,
            { Defs::TEMPLATE_FILE_DIR + Defs::SPECTRA_FILE_TEMPLATE + Defs::TEMPLATE_FILE_EXT,
                &AncillaryFileTest::testSpectraF,
                &AncillaryFileTest::testSpectraS }},
        { FileType::PlanarFit,
            { Defs::TEMPLATE_FILE_DIR + Defs::PLANAR_FIT_FILE_TEMPLATE + Defs::TEMPLATE_FILE_EXT,
                &AncillaryFileTest::testPlanarFitF,
                &AncillaryFileTest::testPlanarFitS }},
        { FileType::TimeLag,
            { Defs::TEMPLATE_FILE_DIR + Defs::TIME_LAG_FILE_TEMPLATE + Defs::TEMPLATE_FILE_EXT,
                &AncillaryFileTest::testTimeLagF,
                &AncillaryFileTest::testTimeLagS }}
        };

    LineList templateLines_ {};
    LineList actualLines_ {};
    QVector<QVector<double>> timelagValues;
    QVector<QVector<double>> h2oTimelagValues;
};

#endif  // ANCILLARYFILETEST_H
