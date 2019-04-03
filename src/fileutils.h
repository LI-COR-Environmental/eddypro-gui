/***************************************************************************
  fileutils.h
  -----------
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

#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <QDateTime>
#include <QDirIterator>
#include <QPair>
#include <QString>

namespace FileUtils
{
    using DateRange = QPair<QDateTime, QDateTime>;

    bool existsPath(const QString& p);

    bool isFileEmpty(const QString& fileName);
    bool isDirEmpty(const QString& dirName);

    bool projectFileForcedCopy(const QString& fileName,
                               const QString& destDir);

    void createDir(const QString& dirName, const
                   QString& absoluteDirDest = QString());

    bool removeDirRecursively(const QString& dirName);
    void cleanDir(const QString& d);
    void cleanDirRecursively(const QString& d);
    void cleanSmfDirRecursively(const QString& appEnvPath);
    void cleanDirFromFiletypeRecursively(const QString& d,
                                          const QStringList& illegalFileTypes);

    const QStringList getFiles(const QString& dir,
                               const QString& extension,
                               bool recurse = false);
    QStringList getDirContent(const QString& dirPath,
                              QStringList nameFilter,
                              QDirIterator::IteratorFlag flag);

    void chmod_644(const QString& filename);

    QDate getDateFromDoY(int doy, int year);
    QDateTime getDateTimeFromFilename(const QString& filename,
                                      const QString& filenameFormat);
    DateRange getDateRangeFromFileList(const QStringList& fileList,
                                       const QString& filenameProtoype);
    bool dateRangesOverlap(DateRange range_1, DateRange range_2);

    QString getGhgSuffixFromFilename(const QString& filename);
    QStringList getGhgFileSuffixList(const QStringList& fileList);

    void loadStyleSheetFile(const QString& sheetPath);
    QString setupEnv();

    bool zipContainsFiletype(const QString& fileName,
                             const QString& filePattern);
    bool zipExtract(const QString& fileName,
                    const QString& outDir);

    bool prependToFile(const QString& str, const QString& filename);
    bool appendToFile(const QString& str, const QString& filename);

} // FileUtils

#endif // FILEUTILS_H
