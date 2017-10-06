/***************************************************************************
  calibrationapi.cpp
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

#include "calibrationapi.h"

#include <QFileInfo>

#include "dbghelper.h"
#include "defs.h"
#include "globalsettings.h"

CalibrationAPI::CalibrationAPI(QObject *parent)
    : QObject(parent),
      manager_(),
      cal_info_download_(nullptr),
      cal_file_download_(nullptr),
      cal_file_()
{
}

void CalibrationAPI::getCalibrationInfo(const QString &serialNumber)
{
    auto url = Defs::CALIBRATION_API_URL;
    url += serialNumber;
    url += QStringLiteral(".json");

    cal_info_download_ = getRequest(url);

    connect(cal_info_download_, &QNetworkReply::finished,
        this, &CalibrationAPI::downloadInfoFinished);
}

void CalibrationAPI::getCalibrationFile(const QString &fileUrl)
{
    QString filename = saveFileName(QUrl(fileUrl));
    cal_file_.setFileName(filename);
    if (!cal_file_.open(QIODevice::WriteOnly))
    {
        qDebug() << cal_file_.errorString();
        return;
    }

    cal_file_download_ = getRequest(fileUrl);

    connect(cal_file_download_, &QNetworkReply::readyRead,
            this, &CalibrationAPI::downloadFileReadyRead);
    connect(cal_file_download_, &QNetworkReply::downloadProgress,
            this, &CalibrationAPI::downloadFileProgress);
    connect(cal_file_download_, &QNetworkReply::finished,
            this, &CalibrationAPI::downloadFileFinished);
}

QNetworkReply* CalibrationAPI::getRequest(const QString &urlString)
{
    QUrl url(urlString);
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", Defs::EP_USER_AGENT.toLatin1());
    return manager_.get(request);
}

void CalibrationAPI::downloadInfoFinished()
{
    if (cal_info_download_->error())
    {
        // A communication error has occurred
        emit networkError(cal_info_download_->error());
        return;
    }

    QByteArray data = cal_info_download_->readAll();
    emit calibrationInfoReady(data);
    cal_info_download_->deleteLater();
}

void CalibrationAPI::downloadFileProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    qDebug() << "Downloaded " << bytesReceived << "of " << bytesTotal;
}

void CalibrationAPI::downloadFileReadyRead()
{
    cal_file_.write(cal_file_download_->readAll());
}

void CalibrationAPI::downloadFileFinished()
{
    cal_file_.close();


    if (cal_file_download_->error())
    {
        // A communication error has occurred
        emit networkError(cal_file_download_->error());
        return;
    }

    emit calibrationFileReady();
    cal_file_download_->deleteLater();
}

QString CalibrationAPI::saveFileName(const QUrl &url)
{
    auto homePath = GlobalSettings::getAppPersistentSettings(
                            Defs::CONFGROUP_GENERAL,
                            Defs::CONF_GEN_ENV,
                            QString()).toString();

    auto calDir = homePath
            + QLatin1Char('/')
            + Defs::CAL_FILE_DIR
            + QLatin1Char('/');

    QString path = url.path();
    QString basename = calDir + QFileInfo(path).fileName();

    // remove existing file and download again to ensure we get
    // the latest calibration info
    if (QFile::exists(basename))
    {
        QFile::remove(basename);
    }

    return basename;
}
