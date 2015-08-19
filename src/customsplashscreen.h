/***************************************************************************
  customsplashscreen.h
  -------------------
  Custom splash screen
  -------------------
  Copyright (C) 2007-2011, Eco2s team, Antonio Forgione
  Copyright (C) 2011-2015, LI-COR Biosciences
  Author: Antonio Forgione

  This file is part of EddyPro (R).

  EddyPro (R) is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  EddyPro (R) is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with EddyPro (R). If not, see <http://www.gnu.org/licenses/>.
****************************************************************************/

#ifndef CUSTOMSPLASHSCREEN_H
#define CUSTOMSPLASHSCREEN_H

#include <QSplashScreen>

class QCheckBox;
class QPainter;
class QPixmap;
class QProgressBar;

class CustomSplashScreen : public QSplashScreen
{
    Q_OBJECT

public:
    explicit CustomSplashScreen(const QPixmap & pixmap = QPixmap(), Qt::WindowFlags f = Qt::Widget);
    ~CustomSplashScreen();

    void setProgressValue(int value);
    void showStatusMessage(const QString &message, const QColor &color = Qt::black);
    void setMessageRect(QRect rect, int alignment = Qt::AlignLeft);

protected:
    void drawContents(QPainter *painter) Q_DECL_OVERRIDE;

signals:

private slots:
    void updateShowSplash(bool notShow);

private:
    QString message_;
    int alignement_;
    QColor color_;
    QRect rect_;
    QProgressBar* progressBar_;
    QCheckBox* showSplashCheckbox_;
};

#endif // CUSTOMSPLASHSCREEN_H
