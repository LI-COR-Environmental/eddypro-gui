/***************************************************************************
  customsplashscreen.cpp
  ----------------------
  Copyright © 2007-2011, Eco2s team, Antonio Forgione
  Copyright © 2011-2019, LI-COR Biosciences, Inc. All Rights Reserved.
  Author: Antonio Forgione

  This file is part of EddyPro®.

  NON-COMMERCIAL RESEARCH PURPOSES ONLY - EDDYPRO® is licensed for
  non-commercial academic and government research purposes only,
  as provided in the EDDYPRO® End User License Agreement.
  EDDYPRO® may only be used as provided in the End User License Agreement
  and may not be used or accessed for any commercial purposes.
  You may view a copy of the End User License Agreement in the file
  EULA_NON_COMMERCIAL.rtf.

  Commercial companies that are LI-COR flux system customers are
  encouraged to contact LI-COR directly for our commercial EDDYPRO®
  End User License Agreement.

  EDDYPRO® contains Open Source Components (as defined in the
  End User License Agreement). The licenses and/or notices for the
  Open Source Components can be found in the file LIBRARIES.txt.

  EddyPro® is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/

#include "customsplashscreen.h"

#include <QCheckBox>
#include <QPainter>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>

#include "defs.h"
#include "globalsettings.h"

CustomSplashScreen::CustomSplashScreen(const QPixmap & pixmap, Qt::WindowFlags f)
    : QSplashScreen(pixmap, f)
{
    progressBar_ = new QProgressBar;
    progressBar_->setRange(0, 100);
    progressBar_->setProperty("splash", true);

    showSplashCheckbox_ = new QCheckBox(tr("Do not show again"));
    showSplashCheckbox_->setObjectName(QStringLiteral("splashCheckbox"));

    auto okButton = new QPushButton;
    okButton->setText(QStringLiteral("Ok"));
    okButton->setProperty("okSplash", true);

    auto rowLayout = new QHBoxLayout;
    rowLayout->addWidget(showSplashCheckbox_, Qt::AlignLeft);
    rowLayout->addWidget(okButton, Qt::AlignCenter);
    rowLayout->addStretch(1);

    auto layout = new QVBoxLayout(this);
    layout->addSpacerItem(new QSpacerItem(1, 335));
    layout->addLayout(rowLayout);
    layout->addWidget(progressBar_, 1, Qt::AlignVCenter);
    layout->addStretch(18);
    layout->setContentsMargins(60, 0, 60, 24);
    layout->setSpacing(0);
    setLayout(layout);

    connect(showSplashCheckbox_, &QCheckBox::toggled,
            this, &CustomSplashScreen::updateShowSplash);
    connect(okButton, &QPushButton::clicked,
            this, &QSplashScreen::close);
}

void CustomSplashScreen::drawContents(QPainter *painter)
{
    painter->setPen(color_);
    painter->drawText(rect_, alignement_, message_);
    update();
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
    update();
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
