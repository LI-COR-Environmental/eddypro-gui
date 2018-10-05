#include "qmlwidget.h"

QmlWidget::QmlWidget(QUrl source, QWidget* parent) :
    QQuickWidget(parent)
{
    setSource(QUrl(std::move(source)));
    setResizeMode(QQuickWidget::SizeRootObjectToView);
}
