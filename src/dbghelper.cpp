/***************************************************************************
  dbghelper.cpp
  -------------------
  Helper class for debugging
  -------------------
  Copyright (C) 2009 Witold Wysota

  from http://blog.wysota.eu.org/index.php/2009/11/17/little-debugging-helper/

  Modified by Antonio Forgione, 2011
****************************************************************************/

#include <QTime>

#include "dbghelper.h"

#ifdef DBGHELPER_USES_PRINTF
#include <cstdio>
#else
#include <QDebug>
#endif

int DbgHelper::indent = 0;
int DbgHelper::colorIndex = 0;

static void DbgHelper_output(int color, int indent,
                             const QString &prefix, const QString &funcName) {
  QString text = QString(4*indent, QLatin1Char(' ')) + QString(prefix + QLatin1Char(' ') + funcName);
  if (color >= 0)
  {
    text.prepend(QStringLiteral("\x1b[3") + QString::number(1 + color) + QStringLiteral("m"));
//    QString colorNum = QString::number(1 + color);
//    text.prepend(QLatin1String("\x1b[3") + colorNum + QLatin1String("m;3") + colorNum + QLatin1String(";3") + colorNum + QLatin1String("m"));
    text.append(QStringLiteral("\x1b[39m"));
  }

  QTime currTime = QTime::currentTime();

#ifndef DBGHELPER_USES_PRINTF
  qDebug() << currTime.toString(QStringLiteral("hh:mm:ss:zzz")) << " " << text;
#else
  fprintf(stderr, "%s\n", qPrintable(text));
#endif
}

DbgHelper::DbgHelper(const QString &t) {
    txt = t;
#ifdef NO_COLOR
    myColor=-1;
#else
    myColor = colorIndex;
    colorIndex = (colorIndex + 1) % 7;
#endif
    DbgHelper_output(myColor, indent, QStringLiteral("BEGIN"), txt);
    ++indent;
}

DbgHelper::~DbgHelper() {
    --indent;
    DbgHelper_output(myColor, indent, QStringLiteral("END"), txt);
}
