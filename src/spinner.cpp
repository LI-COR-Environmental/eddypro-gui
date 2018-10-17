#include "spinner.h"

#include <QQuickItem>

Spinner::Spinner(QWidget *parent) :
    QmlWidget (QUrl(), parent)
{
    this->setSource(QUrl(QStringLiteral(":/quick_widgets/spinner")));
}

void Spinner::start()
{
    this->rootObject()->setProperty("running", true);
}

void Spinner::stop()
{
    this->rootObject()->setProperty("running", false);
}
