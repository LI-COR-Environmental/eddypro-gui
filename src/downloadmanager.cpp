/***************************************************************************
  downloadmanager.cpp
  -------------------
  Copyright © 2016-2019, LI-COR Biosciences, Inc. All Rights Reserved.
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
