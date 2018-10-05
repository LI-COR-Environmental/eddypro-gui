#ifndef SPINNER_H
#define SPINNER_H

#include "qmlwidget.h"

class Spinner : public QmlWidget
{
    Q_OBJECT

public:
    explicit Spinner(QWidget *parent = nullptr);

    void start();
    void stop();
};

#endif // SPINNER_H
