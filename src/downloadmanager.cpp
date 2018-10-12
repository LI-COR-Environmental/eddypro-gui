/***************************************************************************
  downloadmanager.cpp
  -------------------
  Copyright (C) 2016-2018, LI-COR Biosciences
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

#include "downloadmanager.h"

#include <QDebug>
#include <QNetworkRequest>
#include <QUrl>

#include <QThread>

#include "defs.h"

DownloadManager::DownloadManager(QObject *parent) :
    QObject(parent),
    reply(nullptr),
    versionNr(QByteArray())
{
}

void DownloadManager::abort()
{
    if (reply)
    {
        if (reply->isRunning())
        {
            // TODO(emiola): remove test when bump Qt version on Windows
#if (QT_VERSION > 0x050302) && (__GNUC__ >= 4) && (__GNUC_MINOR__ > 8)
            disconnect(reply, &QNetworkReply::finished,
                       this, &DownloadManager::downloadFinished);
#else
            disconnect(reply, SIGNAL(QNetworkReply::finished),
                       this, SLOT(DownloadManager::downloadFinished));
#endif
            reply->abort();
        }
        reply->deleteLater();
    }
}

void DownloadManager::execute()
{
    QUrl url = QUrl::fromEncoded(Defs::LICOR_EP_VERSION_FILE.toLocal8Bit());
    get(url);
}

void DownloadManager::get(const QUrl &url)
{
    QNetworkRequest request;
    request.setUrl(url);
    request.setRawHeader("User-Agent", Defs::EP_USER_AGENT.toLatin1());

    reply = manager.get(request);

    connect(reply, &QNetworkReply::downloadProgress,
            this, &DownloadManager::downloadProgress);
    connect(reply, &QNetworkReply::finished,
            this, &DownloadManager::downloadFinished);
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(downloadError(QNetworkReply::NetworkError)));
}

void DownloadManager::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    qDebug() << "Downloaded " << bytesReceived << "of " << bytesTotal;
}

void DownloadManager::downloadError(QNetworkReply::NetworkError error)
{
    qDebug() << "Reply error" << error;
    qDebug() << reply->errorString();
    versionNr = QByteArray();
}

void DownloadManager::downloadFinished()
{
    if (reply)
    {
        if (reply->error())
        {
            qDebug() << "Request Failed: error" << reply->error();
        }
        else
        {
            qDebug() << "Request Succeded";
        }

        if (!reply->error())
        {
            versionNr = reply->readAll();
        }

        reply->deleteLater();
        emit downloadComplete();
    }
}

QByteArray DownloadManager::getVersionNr() const
{
    return versionNr;
}
