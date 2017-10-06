/***************************************************************************
  calibrationapi.h
  -------------------
  -------------------
  Copyright (C) 2016-2017, LI-COR Biosciences
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

#ifndef CALIBRATIONAPI_H
#define CALIBRATIONAPI_H

#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>

class CalibrationAPI : public QObject
{
    Q_OBJECT

public:
    explicit CalibrationAPI(QObject *parent = 0);

    void getCalibrationInfo(const QString &serialNumber);
    void getCalibrationFile(const QString &fileUrl);

signals:
    void networkError(QNetworkReply::NetworkError err);

    void calibrationInfoReady(const QByteArray &calibrationAsJson);
    void calibrationFileReady();

private slots:
    void downloadInfoFinished();

    void downloadFileReadyRead();
    void downloadFileProgress(qint64 bytesReceived, qint64 bytesTotal);
    void downloadFileFinished();

private:
    QNetworkReply *getRequest(const QString &urlString);

    QString saveFileName(const QUrl &url);

    QNetworkAccessManager manager_;

    QNetworkReply* cal_info_download_;
    QNetworkReply* cal_file_download_;
    QFile cal_file_;
};

#endif  // CALIBRATIONAPI_H
