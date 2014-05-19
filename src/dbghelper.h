/***************************************************************************
  dbghelper.h
  -------------------
  Helper class for debugging
  -------------------
  Copyright (C) 2009 Witold Wysota

  from http://blog.wysota.eu.org/index.php/2009/11/17/little-debugging-helper/

  Modified by Antonio Forgione, 2011
****************************************************************************/

#ifndef DBGHELPER_H

#include <QString>
#include <QMessageBox>

#ifndef QT_NO_DEBUG
#    define DEBUG_FUNC_NAME DbgHelper dbgHelper(QLatin1String(Q_FUNC_INFO));
#    define DEBUG_FUNC_MSG(x) QMessageBox::warning(0, QLatin1String(Q_FUNC_INFO), x);
#else
#    define DEBUG_FUNC_NAME
#    define DEBUG_FUNC_MSG(x)
#endif

#ifdef Q_OS_WIN
#    define NO_COLOR
#endif

// Useful class for call function tracing
class DbgHelper {
public:
    DbgHelper(const QString &t);
    ~DbgHelper();
private:
    QString txt;
    static int indent;
    static int colorIndex;
    int myColor;
};
#endif
