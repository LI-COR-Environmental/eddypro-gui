/***************************************************************************
  ftpmanager.cpp
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
 ***************************************************************************/

#include "ftpmanager.h"

#include <QDebug>
#include <QFile>
#include <QNetworkRequest>
#include <QUrl>

#include "dbghelper.h"
#include "defs.h"

FtpManager::FtpManager(QObject *parent) :
    QObject(parent),
    reply(nullptr),
    data(nullptr),
    versionNr(QByteArray())
{
    connect(&manager, &QNetworkAccessManager::finished,
            this, &FtpManager::transferFinished);
}

FtpManager::~FtpManager()
{
}

void FtpManager::abort()
{
    DEBUG_FUNC_NAME
    if (reply)
    {
        if (reply->isRunning())
        {
            disconnect(reply, &QNetworkReply::finished,
                       this, &FtpManager::requestFinished);
            reply->abort();
        }
        reply->deleteLater();
    }
}

void FtpManager::get(const QString &file)
{
    QUrl url(Defs::LICOR_FTP_URL + file);
    url.setUserName(Defs::LICOR_FTP_USERNAME);
    url.setPassword(Defs::LICOR_FTP_PASSWORD);

    reply = manager.get(QNetworkRequest(url));

    connect(reply, &QNetworkReply::downloadProgress,
            this, &FtpManager::downloadProgress);
    connect(reply, &QNetworkReply::finished,
            this, &FtpManager::requestFinished);
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(requestError(QNetworkReply::NetworkError)));
}

void FtpManager::put(const QString &file)
{
    QUrl url(Defs::LICOR_FTP_URL + file);
    url.setUserName(Defs::LICOR_FTP_USERNAME);
    url.setPassword(Defs::LICOR_FTP_PASSWORD);

    data = new QFile(file, this);

    if (data->open(QIODevice::ReadOnly))
    {
        reply = manager.put(QNetworkRequest(url), data);

        connect(reply, &QNetworkReply::uploadProgress,
                this, &FtpManager::uploadProgress);
        connect(reply, &QNetworkReply::finished,
                this, &FtpManager::uploadDone);
        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
                this, SLOT(requestError(QNetworkReply::NetworkError)));
    }
    else
    {
        qDebug() << "Problem";
        qDebug() << reply->errorString();
    }
}

void FtpManager::transferFinished(QNetworkReply *r)
{
    qDebug() << "Transfer Finished" << r->error();

    if (data)
    {
        data->deleteLater();
    }
}

void FtpManager::requestFinished()
{
    if (reply)
    {
        qDebug() << "Request Finished" << reply->error();

        versionNr = reply->readAll();
        qDebug() << "versionNr" << versionNr.trimmed().constData();

        emit requestComplete();
    }
}

void FtpManager::requestError(QNetworkReply::NetworkError error)
{
    qDebug() << "Reply error" << error;
    qDebug() << reply->errorString();
}

void FtpManager::execute()
{
    DEBUG_FUNC_NAME
    get(QStringLiteral("current-version"));
}

void FtpManager::uploadProgress(qint64 bytesSent, qint64 bytesTotal) {
    qDebug() << "Uploaded " << bytesSent << "of " << bytesTotal;
}

void FtpManager::downloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
    qDebug() << "Downloaded " << bytesReceived << "of " << bytesTotal;
}

void FtpManager::uploadDone()
{
    qDebug() << "Upload Done" << reply->error();
}

QByteArray FtpManager::getVersionNr() const
{
    return versionNr;
}
