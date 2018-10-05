#include "qmlwidget.h"

QmlWidget::QmlWidget(QUrl source, QWidget* parent) :
    QQuickWidget(parent)
{
    setSource(QUrl(source));
    setResizeMode(QQuickWidget::SizeRootObjectToView);
}
