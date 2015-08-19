/***************************************************************************
  customsplashscreen.cpp
  -------------------
  Custom splash screen
  -------------------
  Copyright (C) 2007-2011, Eco2s team, Antonio Forgione
  Copyright (C) 2011-2014, LI-COR Biosciences
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

#include "customsplashscreen.h"

#include <QCheckBox>
#include <QPainter>
#include <QProgressBar>
#include <QVBoxLayout>

#include "defs.h"
#include "globalsettings.h"

CustomSplashScreen::CustomSplashScreen(const QPixmap & pixmap, Qt::WindowFlags f) :
    QSplashScreen(pixmap, f)
{
    progressBar_ = new QProgressBar(this);
    progressBar_->setRange(0, 100);
    progressBar_->setProperty("splash", true);

    showSplashCheckbox_ = new QCheckBox(tr("Do not show again"));

    auto layout = new QVBoxLayout;
    layout->addSpacerItem(new QSpacerItem(100, 350));
    layout->addWidget(progressBar_);
    layout->addWidget(showSplashCheckbox_);
    layout->addStretch();
    layout->setMargin(50);
    setLayout(layout);

    connect(showSplashCheckbox_, &QCheckBox::toggled,
            this, &CustomSplashScreen::updateShowSplash);
}

CustomSplashScreen::~CustomSplashScreen()
{
}

void CustomSplashScreen::drawContents(QPainter *painter)
{
    painter->setPen(color_);
    painter->drawText(rect_, alignement_, message_);
}

void CustomSplashScreen::setProgressValue(int value)
{
    progressBar_->setValue(value);
}

void CustomSplashScreen::showStatusMessage(const QString &message, const QColor &color)
{
    message_ = message;
    color_ = color;
    showMessage(message_, alignement_, color_);
}

void CustomSplashScreen::setMessageRect(QRect rect, int alignement)
{
    rect_ = rect;
    alignement_ = alignement;
}

void CustomSplashScreen::updateShowSplash(bool notShow)
{
    GlobalSettings::setAppPersistentSettings(Defs::CONFGROUP_GENERAL,
                                             Defs::CONF_GEN_SHOW_SPLASH,
                                             !notShow);
}
