#ifndef LOGGING_H
#define LOGGING_H

#include <QtCore/QMessageLogContext>

namespace Logging
{
    void messageHandler(QtMsgType type,
                        const QMessageLogContext& context,
                        const QString& message);
};

#endif // LOGGING_H
