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
#define DBGHELPER_H

#include <QMessageBox>
#include <QString>

////////////////////////////////////////////////////////////////////////////////
/// \file src/dbghelper.h
/// \brief DbgHelper class
/// \version     1.0
/// \date        2009-11-17
/// \author      Witold Wysota
/// \note Modified by Antonio Forgione
/// \sa http://blog.wysota.eu.org/index.php/2009/11/17/little-debugging-helper/
/// \bug
/// \deprecated
/// \test
/// \todo
////////////////////////////////////////////////////////////////////////////////

#ifndef QT_NO_DEBUG
#    define DEBUG_FUNC_NAME DbgHelper dbgHelper(QLatin1String(Q_FUNC_INFO));
#    define DEBUG_FUNC_MSG(x) QMessageBox::warning(nullptr, QLatin1String(Q_FUNC_INFO), x);
#else
#    define DEBUG_FUNC_NAME
#    define DEBUG_FUNC_MSG(x)
#endif

#ifdef Q_OS_WIN
#    define NO_COLOR
#endif

/// \class DbgHelper
/// \brief Useful class for call function tracing
class DbgHelper {
public:
    explicit DbgHelper(const QString &t);
    ~DbgHelper();
private:
    QString txt;
    static int indent;
    static int colorIndex;
    int myColor;
};
#endif
