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

#endif // CALIBRATIONAPI_H
