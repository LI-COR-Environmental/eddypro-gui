/***************************************************************************
  customheader.cpp
  ----------------
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

#include "customheader.h"

#include <QGridLayout>
#include <QUrl>

#include "widget_utils.h"

CustomHeader::CustomHeader(QWidget *parent) :
    QWidget(parent)
{
    layout = new QGridLayout(this);
    layout->setVerticalSpacing(0);
    setLayout(layout);
}

void CustomHeader::addSection(const QString &txt,
                              const QString &tooltipTxt,
                              QuestionMarkHint questionMark,
                              ClickLabel::HeaderData headerData)
{
    auto headerTextLabel = new QLabel;
    headerTextLabel->setText(txt);
    headerTextLabel->setToolTip(tooltipTxt);

    layout->addWidget(headerTextLabel, layout->rowCount(), 0);

    if (questionMark == QuestionMarkHint::QuestionMark)
    {
        auto questionMarkLabel = new ClickLabel;

        auto pixmap_2x = QPixmap(QStringLiteral(":/icons/qm-enabled"));
#if defined(Q_OS_MACOS)
        pixmap_2x.setDevicePixelRatio(2.0);
#endif
        questionMarkLabel->setPixmap(pixmap_2x);
        questionMarkLabel->setHeaderData(headerData);
        connect(questionMarkLabel, &ClickLabel::clicked,
                this, &CustomHeader::onlineHelpTrigger);

        layout->addWidget(questionMarkLabel, layout->rowCount() - 1, 1);
    }
}

void CustomHeader::onlineHelpTrigger()
{
    auto cLabel = qobject_cast<ClickLabel *>(sender());

    ClickLabel::HeaderData data = cLabel->headerData();

    if (data == ClickLabel::AnemSwVersion)
    {
        WidgetUtils::showHelp(QUrl(QStringLiteral("http://www.licor.com/env/support/EddyPro/topics/sonic-anemometer-firmware-version.html")));
    }
    else if (data == ClickLabel::AnemNAlign)
    {
        WidgetUtils::showHelp(QUrl(QStringLiteral("http://www.licor.com/env/support/EddyPro/topics/axes-alignment.html")));
    }
    else if (data == ClickLabel::AnemNOffset)
    {
        WidgetUtils::showHelp(QUrl(QStringLiteral("http://www.licor.com/env/support/EddyPro/topics/north-offset.html")));
    }
    else if (data == ClickLabel::AnemNSep
             || data == ClickLabel::AnemESep
             || data == ClickLabel::AnemVSep)
    {
        WidgetUtils::showHelp(QUrl(QStringLiteral("http://www.licor.com/env/support/EddyPro/topics/sensor-separation.html")));
    }
    else if (data == ClickLabel::IrgaModel)
    {
        WidgetUtils::showHelp(QUrl(QStringLiteral("http://www.licor.com/env/support/EddyPro/topics/gas-analyzer-model.html")));
    }
    else if (data == ClickLabel::IrgaLPathLength
             || data == ClickLabel::IrgaTPathLength)
    {
        WidgetUtils::showHelp(QUrl(QStringLiteral("http://www.licor.com/env/support/EddyPro/topics/longitude-transverse-pathlengths.html")));
    }
    else if (data == ClickLabel::VarIgnoreDesc
             || data == ClickLabel::VarNumericDesc
             || data == ClickLabel::VarDesc)
    {
        WidgetUtils::showHelp(QUrl(QStringLiteral("http://www.licor.com/env/support/EddyPro/topics/sensitive-and-nonsensitive-variables.html")));
    }
    else if (data == ClickLabel::VarConv
             || data == ClickLabel::VarMinValue
             || data == ClickLabel::VarMaxValue)
    {
        WidgetUtils::showHelp(QUrl(QStringLiteral("http://www.licor.com/env/support/EddyPro/topics/conversion-type.html")));
    }
    else if (data == ClickLabel::VarNomTLag
             || data == ClickLabel::VarMinTLag
             || data == ClickLabel::VarMaxTLag)
    {
        WidgetUtils::showHelp(QUrl(QStringLiteral("http://www.licor.com/env/support/EddyPro/topics/nominal-time-lag.html")));
    }
}

int CustomHeader::sectionCount()
{
    return layout->rowCount();
}
