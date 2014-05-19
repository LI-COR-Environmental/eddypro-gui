/***************************************************************************
  fileutils.h
  -------------------
  Copyright (C) 2011-2014, LI-COR Biosciences
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

#include <QString>
#include <QDateTime>
#include <QPair>
#include <QDirIterator>

class FileUtils
{
public:
    static bool isEmptyFile(const QString& fileName);
    static bool projectFileForcedCopy(const QString& fileName, const QString &destDir);

    static void createDir(const QString& dirName, const QString& absoluteDirDest = QString());
    static bool removeDir(const QString& dirName);
    static bool isDirEmpty(const QString& dirName);
    static bool existsPath(const QString& p);
    static void cleanDirRecursive(const QString& d);
    static void cleanDirFromFileTypes(const QString& d, const QStringList& illegalFileTypes);

    static const QStringList getFiles(const QString& dir,
                                const QString& extension,
                                bool recurse);
    static QStringList getDirContent(const QString& dirPath,
                                     QStringList nameFilter,
                                     QDirIterator::IteratorFlag flag);

    static QDate getDateFromDoY(int doy, int year);
    static QDateTime getDateTimeFromFilename(const QString &filename, const QString &filenameFormat);
    static QPair<QDateTime, QDateTime> getDateRangeFromFileList(const QStringList &fileList, const QString &filenameProtoype);

    static void loadStyleSheetFile(const QString &sheetPath);
    static QString setupEnv();

    static bool zipContainsFiletype(const QString &fileName, const QString &filePattern);
    static bool zipExtract(const QString &fileName, const QString &outDir);
    static void cleanSmfDir(const QString &appEnvPath);
    static bool forcedFileCopy(const QString &fileName, const QString &destDir);
    static void cleanDir(const QString &d);
};

#endif // FILEUTILS_H
