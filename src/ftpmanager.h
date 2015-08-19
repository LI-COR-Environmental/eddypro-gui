/***************************************************************************
  ftpmanager.h
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

#ifndef FTPMANAGER_H
#define FTPMANAGER_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>

class QFile;
class QNetworkReply;

class FtpManager : public QObject
{
    Q_OBJECT

    QNetworkAccessManager manager;
    QNetworkReply *reply;
    QFile* data;

public:
    explicit FtpManager(QObject* parent = nullptr);
    ~FtpManager();

    void put(const QString &file);
    void get(const QString &file);
    void abort();

    QByteArray getVersionNr() const;

signals:
    void getFinished();

public slots:
    void execute();

private slots:
    void transferFinished(QNetworkReply *reply);
    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void requestError(QNetworkReply::NetworkError error);
    void requestFinished();
    void uploadDone();
private:
    QByteArray versionNr;
};

#endif // FTPMANAGER_H
