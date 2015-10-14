/***************************************************************************
  fileutils.h
  -------------------
  Copyright (C) 2011-2015, LI-COR Biosciences
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

    bool fileForcedCopy(const QString& fileName,
                        const QString& destDir);
    bool projectFileForcedCopy(const QString& fileName,
                               const QString& destDir);


    void createDir(const QString& dirName, const
                   QString& absoluteDirDest = QString());

    bool removeDirRecursively(const QString& dirName);
    void cleanDir(const QString& d);
    void cleanDirRecursively(const QString& d);
    void cleanDirRecursively_alt(const QString& d);
    void cleanSmfDirRecursively(const QString& appEnvPath);
    void cleanDirFromFileTypesRecursively(const QString& d,
                                          const QStringList& illegalFileTypes);

    const QStringList getFiles(const QString& dir,
                               const QString& extension,
                               bool recurse);
    QStringList getDirContent(const QString& dirPath,
                              QStringList nameFilter,
                              QDirIterator::IteratorFlag flag);

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

} // FileUtils

#endif // FILEUTILS_H
