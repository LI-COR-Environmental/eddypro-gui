/***************************************************************************
  ancillaryfiletest.h
  -------------------
  Copyright © 2014-2019, LI-COR Biosciences, Inc. All Rights Reserved.
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

    AncillaryFileTest(FileType type, QWidget* parent = nullptr);

    bool makeTest();
    void refresh(const QString &file);

private slots:
    void saveResults();

private:
    using LineList = QList<QStringList>;

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

    QString typeToString(FileType type);

    FileType type_;

    QString name_ {};
    QTextBrowser* testResults_ {};

    // NOTE: we might use something else, for example std::unordered_map
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
