#include "openfilefilter.h"

#include <QFileOpenEvent>

OpenFileFilter::OpenFileFilter(QObject *parent) :
    QObject(parent),
    file_requested_(QString())
{

}

bool OpenFileFilter::eventFilter(QObject* object, QEvent* event)
{
    if (event->type() == QEvent::FileOpen)
    {
        const auto file_event = static_cast<QFileOpenEvent *>(event);
        file_requested_ = file_event->file();
        return true;
    }
    return QObject::eventFilter(object, event);
}

const QString& OpenFileFilter::fileRequested() const
{
    return file_requested_;
}
