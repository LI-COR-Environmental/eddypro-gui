#include "calibrationapi.h"

#include <QFileInfo>

#include "dbghelper.h"
#include "defs.h"
#include "globalsettings.h"

CalibrationAPI::CalibrationAPI(QObject *parent) : QObject(parent)
{
}

void CalibrationAPI::getCalibrationInfo(const QString &serialNumber)
{
    DEBUG_FUNC_NAME
    qDebug() << serialNumber;

    auto url = Defs::CALIBRATION_API_URL;
    url += serialNumber;
    url += QStringLiteral(".json");

    cal_info_download_ = getRequest(url);

    connect(cal_info_download_, &QNetworkReply::finished,
        this, &CalibrationAPI::downloadInfoFinished);
}

void CalibrationAPI::getCalibrationFile(const QString &fileUrl)
{
    DEBUG_FUNC_NAME
    qDebug() << fileUrl;

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
    DEBUG_FUNC_NAME

    QUrl url(urlString);
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", Defs::EP_USER_AGENT.toLatin1());
    return manager_.get(request);
}

void CalibrationAPI::downloadInfoFinished()
{
    DEBUG_FUNC_NAME

    if (cal_info_download_->error())
    {
        qDebug() << cal_info_download_->errorString();

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
    DEBUG_FUNC_NAME
    cal_file_.close();


    if (cal_file_download_->error())
    {
        qDebug() << cal_file_download_->errorString();

        // A communication error has occurred
        emit networkError(cal_file_download_->error());
        return;
    }

    emit calibrationFileReady();
    cal_file_download_->deleteLater();
}

QString CalibrationAPI::saveFileName(const QUrl &url)
{
    DEBUG_FUNC_NAME

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
