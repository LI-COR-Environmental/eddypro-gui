/***************************************************************************
  fileutils.cpp
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

#include "fileutils.h"

#include <QApplication>
#include <QCoreApplication>
#include <QDebug>
#include <QProcessEnvironment>
#include <QtConcurrentRun>

#include "JlCompress.h"

#include "dbghelper.h"
#include "defs.h"
#include "widget_utils.h"

bool FileUtils::isFileEmpty(const QString& fileName)
{
    QFile f(fileName);
    QByteArray fileContentBa = f.readAll();

    // NOTE: conversion from QByteArray to QString
    const char* fileContent = fileContentBa.constData();
    const QString contents = QString::fromUtf8(fileContent);

    f.close();

    if (contents.isEmpty())
    {
        return true;
    }
    return false;
}

bool FileUtils::projectFileForcedCopy(const QString& fileName,
                                      const QString& destDir)
{
    DEBUG_FUNC_NAME
    qDebug() << "fileName" << fileName;
    qDebug() << "destDir" << destDir;

    QString destFile = destDir
                       + QStringLiteral("/")
                       + Defs::DEFAULT_PROCESSING_FILENAME;
    qDebug() << "destFile" << destFile;

    if (QFile::exists(destFile))
    {
        qDebug() << "destFile exist: true";
        QFile::remove(destFile);
    }
    bool res = QFile::copy(fileName, destFile);
    Q_ASSERT(res);
    return res;
}

bool FileUtils::fileForcedCopy(const QString& fileName, const QString& destDir)
{
    DEBUG_FUNC_NAME
    qDebug() << "fileName" << fileName;
    qDebug() << "destDir" << destDir;

    QString destFile = destDir + QStringLiteral("/") + fileName;
    qDebug() << "destFile" << destFile;

    if (QFile::exists(destFile))
    {
        qDebug() << "destFile exist: true";
        QFile::remove(destFile);
    }
    return QFile::copy(fileName, destFile);
}

// creates dirName, which can be absolute or relative if absoluteDirDest is provided
void FileUtils::createDir(const QString& dirName, const QString& absoluteDirDest)
{
    QString dirPath;
    if (!absoluteDirDest.isEmpty())
        dirPath = absoluteDirDest + QStringLiteral("/") + dirName;
    else
        dirPath = dirName;

    QDir dir;
    if (!dir.exists(dirPath))
    {
        bool created = dir.mkdir(dirPath);
        if (created)
        {
            qDebug() << "Created dir" << dirPath;
        }
        else
        {
            qDebug() << "Failed creating dir" << dirPath;
            WidgetUtils::warning(nullptr,
                                 QObject::tr("mkdir error"),
                                 QObject::tr("Error creating dir %1").arg(dirPath));
        }
    }
}

// Delete a directory along with all of its contents.
//
// \param dirName Path of directory to remove.
// \return true on success; false on error.
bool FileUtils::removeDirRecursively(const QString &dirName)
{
    DEBUG_FUNC_NAME

    bool result = true;
    QDir dir(dirName);

    if (dir.exists(dirName))
    {
        foreach (const QFileInfo& info, dir.entryInfoList(QDir::NoDotAndDotDot
                                                    | QDir::System
                                                    | QDir::Hidden
                                                    | QDir::AllDirs
                                                    | QDir::Files,
                                                      QDir::DirsFirst))
        {
            QString absolutePath = info.absoluteFilePath();
            if (info.isDir())
            {
                // recursion
                result = removeDirRecursively(absolutePath);
            }
            else
            {
                result = QFile::remove(absolutePath);
            }

            if (!result)
            {
                return result;
            }
        }
        result = dir.rmdir(dirName);
    }

    return result;
}

// test if the path string p exists
bool FileUtils::existsPath(const QString& p)
{
    if (!p.isEmpty())
    {
        QDir dir(p);
        if (dir.exists())
            return true;
    }
    return false;
}

bool FileUtils::isDirEmpty(const QString& dirName)
{
    QDir dir(dirName);
    QStringList list = dir.entryList(QDir::Dirs | QDir::Files);
    return list.isEmpty();
}

void FileUtils::cleanDirRecursively(const QString& d)
{
    QDir dir(d);
    dir.removeRecursively();
    createDir(d);
}

void FileUtils::cleanDirRecursively_alt(const QString& d)
{
    if (existsPath(d))
    {
        if (!isDirEmpty(d))
        {
            removeDirRecursively(d);
            createDir(d);
        }
    }
}

void FileUtils::cleanDir(const QString& d)
{
    if (existsPath(d))
    {
        if (!isDirEmpty(d))
        {
            QDir dir(d);
            foreach (const QFileInfo& fileInfo, dir.entryInfoList(QDir::Files
                                                                  | QDir::NoDotAndDotDot))
            {
                QFile::remove(fileInfo.absoluteFilePath());
            }
        }
    }
}

// recursive
void FileUtils::cleanDirFromFileTypesRecursively(const QString &d, const QStringList &illegalFileTypes)
{
    QDirIterator it(d, QDirIterator::Subdirectories);

    while (it.hasNext())
    {
        qDebug() << "Processing: " <<it.next();

        bool illegalFile = false;

        foreach (const QString &illegalType, illegalFileTypes)
        {
            if (it.fileInfo().absoluteFilePath().endsWith(illegalType, Qt::CaseInsensitive))
            {
                illegalFile = true;
                break;
            }
            else
            {
                illegalFile = false;
            }
        }

        if (illegalFile)
        {
            QDir dir;
            dir.remove(it.filePath());
            qDebug() << "Removed file.";
        }
    }
}

const QStringList FileUtils::getFiles(const QString& dir, const QString& extension, bool recurse)
{
    DEBUG_FUNC_NAME

    qDebug() << "params:" << dir << extension << recurse;
    QStringList filters;
    filters << extension;

    QStringList fileList;

    if (recurse)
    {
        QFuture<QStringList> future = QtConcurrent::run(&getDirContent, dir, filters, QDirIterator::Subdirectories);
        while (!future.isFinished())
        {
            QCoreApplication::processEvents();
        }
        fileList = future.result();
    }
    else
    {
        QFuture<QStringList> future = QtConcurrent::run(&getDirContent, dir, filters, QDirIterator::NoIteratorFlags);
        while (!future.isFinished())
        {
            QCoreApplication::processEvents();
        }
        fileList = future.result();
    }
    return fileList;
}

QStringList FileUtils::getDirContent(const QString& dirPath,
                                QStringList nameFilter,
                                QDirIterator::IteratorFlag flag)
{
    DEBUG_FUNC_NAME

    // test empty list
    if (nameFilter.isEmpty()) return QStringList();

    QString extension = nameFilter.first();

    // remove "*."
    extension.remove(0, 2);

    // append the filtered files to this list
    QStringList list;

    // set dir iterator
    QDirIterator dirIterator(dirPath,
                             nameFilter,
                             QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot,
                             flag);

    while (dirIterator.hasNext())
    {
        dirIterator.next();

        // found required file type
        if (dirIterator.fileInfo().completeSuffix().contains(extension, Qt::CaseInsensitive))
        {
            list.append(dirIterator.fileInfo().absoluteFilePath());
        }
    }

    return list;
}

QDate FileUtils::getDateFromDoY(int doy, int year)
{
    QDate date(year, 1, 1);
    return date.addDays(doy - 1);
}

QDateTime FileUtils::getDateTimeFromFilename(const QString& filename, const QString& filenameFormat)
{
    QHash<QString, int> hash;

    QString yyStr(QStringLiteral("yy"));
    QString yyyyStr(QStringLiteral("yyyy"));
    QString mmStr(QStringLiteral("mm"));
    QString ddStr(QStringLiteral("dd"));
    QString dddStr(QStringLiteral("ddd"));
    QString HHStr(QStringLiteral("HH"));
    QString MMStr(QStringLiteral("MM"));

    int yy = 0;
    int yyyy = 0;
    int mm = 0;
    int dd = 0;
    int ddd = 0;
    int HH = 0;
    int MM = 0;

    hash.insert(yyStr, filenameFormat.indexOf(yyStr));
    hash.insert(yyyyStr, filenameFormat.indexOf(yyyyStr));
    hash.insert(mmStr, filenameFormat.indexOf(mmStr));
    hash.insert(ddStr, filenameFormat.indexOf(ddStr));
    hash.insert(dddStr, filenameFormat.indexOf(dddStr));
    hash.insert(HHStr, filenameFormat.indexOf(HHStr));
    hash.insert(MMStr, filenameFormat.indexOf(MMStr));

    QHashIterator<QString, int> i(hash);
    while (i.hasNext())
    {
        i.next();

        qDebug() << "key" << i.key() << "value" << i.value();

        if (i.value() >= 0)
        {
            if (i.key() == yyStr)
            {
                yy = filename.mid(i.value(), yyStr.length()).toInt();
            }
            else if (i.key() == yyyyStr)
            {
                yyyy = filename.mid(i.value(), yyyyStr.length()).toInt();
            }
            else if (i.key() == mmStr)
            {
                mm = filename.mid(i.value(), mmStr.length()).toInt();
            }
            else if (i.key() == ddStr)
            {
                dd = filename.mid(i.value(), ddStr.length()).toInt();
            }
            else if (i.key() == dddStr)
            {
                ddd = filename.mid(i.value(), dddStr.length()).toInt();
            }
            else if (i.key() == HHStr)
            {
                HH = filename.mid(i.value(), HHStr.length()).toInt();
            }
            else if (i.key() == MMStr)
            {
                MM = filename.mid(i.value(), MMStr.length()).toInt();
            }
        }
    }

    qDebug() << "yy" << yy;
    qDebug() << "yyyy" << yyyy;
    qDebug() << "mm" << mm;
    qDebug() << "dd" << dd;
    qDebug() << "ddd" << ddd;
    qDebug() << "HH" << HH;
    qDebug() << "MM" << MM;

    if (hash.value(yyStr) >= 0 && hash.value(yyyyStr) < 0)
    {
        if (yy > 70)
            yyyy = 1970 + yy;
        else
            yyyy = 2000 + yy;
    }
    qDebug() << "yyyy" << yyyy;

    QDate date;
    if (hash.value(dddStr) >= 0 && hash.value(mmStr) < 0)
    {
        date = getDateFromDoY(ddd, yyyy);
    }
    else
    {
        date = QDate(yyyy, mm, dd);
    }
    qDebug() << "date" << date;

    QTime time(HH, MM);
    qDebug() << "time" << time;

    return QDateTime(date, time);
}

QPair<QDateTime, QDateTime> FileUtils::getDateRangeFromFileList(const QStringList& fileList, const QString& filenameProtoype)
{
    QDateTime dateStart;
    QDateTime dateEnd;
    QList<QDateTime> dateList;

    foreach (const QString& s, fileList)
    {
        QString filename = s.mid(s.lastIndexOf(QLatin1Char('/')) + 1);
        qDebug() << filename;
        QDateTime d = getDateTimeFromFilename(filename, filenameProtoype);
        qDebug() << "d" << d;
        if (d != QDateTime())
            dateList.append(d);
    }

    qSort(dateList.begin(), dateList.end());

    if (!dateList.isEmpty())
    {
        dateStart = dateList.first();
        dateEnd = dateList.last();

        return QPair<QDateTime, QDateTime>(dateStart, dateEnd);
    }
    else
        return QPair<QDateTime, QDateTime>();
}

void FileUtils::loadStyleSheetFile(const QString &sheetPath)
{
    qDebug() << "loadStyleSheet" << sheetPath;
    // using a resource alias
    QFile file(sheetPath);

    if (file.open(QIODevice::ReadOnly))
    {
        QByteArray fileContentBa = file.readAll();
        const char* fileContent = fileContentBa.constData();
        const QString contents = QString::fromLatin1(fileContent);

        qApp->setStyleSheet(contents);
        file.close();
    }
    else
    {
        qDebug() << sheetPath;
        qDebug("\tCSS read error");
    }
}

QString FileUtils::setupEnv()
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

#if defined(Q_OS_WIN)
    QString userHomePath = QDir::fromNativeSeparators(env.value(QStringLiteral("USERPROFILE")).trimmed());
#elif defined(Q_OS_LINUX) || defined(Q_OS_MAC)
    QString userHomePath = env.value(QStringLiteral("HOME")) + QStringLiteral("/.config");
#endif

    qDebug() << "env" << env.value(QStringLiteral("HOME"));
    qDebug() << "userHomePath" << userHomePath;

    if (!userHomePath.isEmpty())
    {
        FileUtils::createDir(Defs::LICOR_ENV_DIR, userHomePath);

        QString licorDir = userHomePath + QStringLiteral("/") + Defs::LICOR_ENV_DIR;
        FileUtils::createDir(Defs::APP_NAME_LCASE, licorDir);

        QString appDir = licorDir + QStringLiteral("/") + Defs::APP_NAME_LCASE;
        FileUtils::createDir(Defs::APP_VERSION_STR, appDir);

        QString appVerDir = appDir + QStringLiteral("/") + Defs::APP_VERSION_STR;
        FileUtils::createDir(Defs::INI_FILE_DIR, appVerDir);
        FileUtils::createDir(Defs::LOG_FILE_DIR, appVerDir);
        FileUtils::createDir(Defs::TMP_FILE_DIR, appVerDir);
        FileUtils::createDir(Defs::SMF_FILE_DIR, appVerDir);

        return appVerDir;
    }
    else
    {
        return QString();
    }
}

bool FileUtils::zipContainsFiletype(const QString& fileName, const QString& filePattern)
{
    DEBUG_FUNC_NAME

    qDebug() << "filePattern" << filePattern;

    QStringList entries = JlCompress::getFileList(fileName);
    qDebug() << "entries" << entries;

    foreach (const QString& item, entries) {
        if (item.contains(filePattern.mid(1)))
            return true;
    }

    return false;
}

bool FileUtils::zipExtract(const QString& fileName, const QString& outDir)
{
    return (!JlCompress::extractDir(fileName, outDir).isEmpty());
}

void FileUtils::cleanSmfDirRecursively(const QString& appEnvPath)
{
    // cleanup smf dir
    QString smfDir = appEnvPath + QStringLiteral("/") + Defs::SMF_FILE_DIR;
    cleanDirRecursively(smfDir);
}
