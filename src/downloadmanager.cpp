/***************************************************************************
  downloadmanager.cpp
  -------------------
  Copyright (C) 2015, LI-COR Biosciences
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

#include "dbghelper.h"
#include "defs.h"

DownloadManager::DownloadManager(QObject *parent) :
    QObject(parent),
    reply(nullptr),
    versionNr(QByteArray())
{
}

DownloadManager::~DownloadManager()
{
}

void DownloadManager::abort()
{
    DEBUG_FUNC_NAME

    auto request = static_cast<QNetworkReply*>(reply);
    QThread::sleep(1);
    qDebug() << "reply still existing" << reply;
    QThread::sleep(1);
    qDebug() << "reply still existing" << reply;
    QThread::sleep(1);
    qDebug() << "reply still existing" << request;

    if (request)
    {
        qDebug() << "reply still existing" << request;


        if (request->isRunning())
        {
//            disconnect(reply, &QNetworkReply::finished,
//                       this, &DownloadManager::downloadFinished);
//            reply->abort();
        }
        request->deleteLater();
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
    qDebug() << "reply" << reply;

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
    DEBUG_FUNC_NAME
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

        qDebug() << reply->header(QNetworkRequest::ContentTypeHeader).toString();
        qDebug() << reply->header(QNetworkRequest::LastModifiedHeader).toDateTime().toString();
        qDebug() << reply->header(QNetworkRequest::ContentLengthHeader).toULongLong();
        qDebug() << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        qDebug() << reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();

        if (!reply->error())
        {
            versionNr = reply->readAll();
            qDebug() << "versionNr" << versionNr.trimmed().constData();
        }

        reply->deleteLater();
        emit downloadComplete();
    }
}

QByteArray DownloadManager::getVersionNr() const
{
    return versionNr;
}
