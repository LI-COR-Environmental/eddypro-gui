#ifndef CALIBRATIONAPI_H
#define CALIBRATIONAPI_H

#include <QObject>

class CalibrationAPI : public QObject
{
    Q_OBJECT
public:
    explicit CalibrationAPI(QObject *parent = 0);

signals:

public slots:
};

#endif // CALIBRATIONAPI_H
