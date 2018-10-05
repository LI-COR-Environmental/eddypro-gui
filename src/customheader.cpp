/***************************************************************************
  customheader.cpp
  -------------------
  Custom header for table view classes
  -------------------
  Copyright (C) 2007-2011, Eco2s team, Antonio Forgione
  Copyright (C) 2011-2018, LI-COR Biosciences
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
        WidgetUtils::showHelp(QUrl(QStringLiteral("http://www.licor.com/env/help/eddypro/topics_eddypro/sonic_firmware_version.html")));
    }
    else if (data == ClickLabel::AnemNAlign)
    {
        WidgetUtils::showHelp(QUrl(QStringLiteral("http://www.licor.com/env/help/eddypro/topics_eddypro/Axes_Alignment.html")));
    }
    else if (data == ClickLabel::AnemNOffset)
    {
        WidgetUtils::showHelp(QUrl(QStringLiteral("http://www.licor.com/env/help/eddypro/topics_eddypro/North_Offset.html")));
    }
    else if (data == ClickLabel::AnemNSep
             || data == ClickLabel::AnemESep
             || data == ClickLabel::AnemVSep)
    {
        WidgetUtils::showHelp(QUrl(QStringLiteral("http://www.licor.com/env/help/eddypro/topics_eddypro/Sensor_Separation.html")));
    }
    else if (data == ClickLabel::IrgaModel)
    {
        WidgetUtils::showHelp(QUrl(QStringLiteral("http://www.licor.com/env/help/eddypro/topics_eddypro/Gas_Analyzer_Model.html")));
    }
    else if (data == ClickLabel::IrgaLPathLength
             || data == ClickLabel::IrgaTPathLength)
    {
        WidgetUtils::showHelp(QUrl(QStringLiteral("http://www.licor.com/env/help/eddypro/topics_eddypro/Longitude_Transverse.html")));
    }
    else if (data == ClickLabel::VarIgnoreDesc
             || data == ClickLabel::VarNumericDesc
             || data == ClickLabel::VarDesc)
    {
        WidgetUtils::showHelp(QUrl(QStringLiteral("http://www.licor.com/env/help/eddypro/topics_eddypro/Sensitive_and_Non-sensitive_Variables.html")));
    }
    else if (data == ClickLabel::VarConv
             || data == ClickLabel::VarMinValue
             || data == ClickLabel::VarMaxValue)
    {
        WidgetUtils::showHelp(QUrl(QStringLiteral("http://www.licor.com/env/help/eddypro/topics_eddypro/Conversion_Type.html")));
    }
    else if (data == ClickLabel::VarNomTLag
             || data == ClickLabel::VarMinTLag
             || data == ClickLabel::VarMaxTLag)
    {
        WidgetUtils::showHelp(QUrl(QStringLiteral("http://www.licor.com/env/help/eddypro/topics_eddypro/Nominal_Time_Lag.html")));
    }
}

int CustomHeader::sectionCount()
{
    return layout->rowCount();
}
