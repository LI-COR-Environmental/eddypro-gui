/***************************************************************************
  customheader.cpp
  -------------------
  Custom header for table view classes
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

#include <QGridLayout>
#include <QUrl>

#include "alia.h"
#include "customheader.h"

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
    QLabel* headerTextLabel = new QLabel;
    headerTextLabel->setText(txt);
    headerTextLabel->setToolTip(tooltipTxt);

    layout->addWidget(headerTextLabel, layout->rowCount(), 0);

    if (questionMark)
    {
        ClickLabel* questionMarkLabel = new ClickLabel;
        questionMarkLabel->setHeaderData(headerData);
        questionMarkLabel->setObjectName(QStringLiteral("questionMarkImg"));
        connect(questionMarkLabel, SIGNAL(clicked()),
                this, SLOT(onlineHelpTrigger()));

        layout->addWidget(questionMarkLabel, layout->rowCount() - 1, 1);
    }
}

void CustomHeader::onlineHelpTrigger()
{
    ClickLabel* cLabel = qobject_cast<ClickLabel *>(sender());

    ClickLabel::HeaderData data = cLabel->headerData();

    if (data == ClickLabel::AnemNAlign)
    {
        Alia::showHelp(QUrl(QStringLiteral("http://envsupport.licor.com/help/EddyPro5/index.htm#Axes_Alignment.htm")));
    }
    else if (data == ClickLabel::AnemNOffset)
    {
        Alia::showHelp(QUrl(QStringLiteral("http://envsupport.licor.com/help/EddyPro5/index.htm#North_Offset.htm")));
    }
    else if (data == ClickLabel::AnemNSep
             || data == ClickLabel::AnemESep
             || data == ClickLabel::AnemVSep)
    {
        Alia::showHelp(QUrl(QStringLiteral("http://envsupport.licor.com/help/EddyPro5/index.htm#Sensor_Separation.htm")));
    }
    else if (data == ClickLabel::IrgaModel)
    {
        Alia::showHelp(QUrl(QStringLiteral("http://envsupport.licor.com/help/EddyPro5/index.htm#Gas_Analyzer_Model.htm")));
    }
    else if (data == ClickLabel::IrgaLPathLength
             || data == ClickLabel::IrgaTPathLength)
    {
        Alia::showHelp(QUrl(QStringLiteral("http://envsupport.licor.com/help/EddyPro5/index.htm#Longitude_Transverse.htm")));
    }
    else if (data == ClickLabel::VarIgnoreDesc
             || data == ClickLabel::VarNumericDesc
             || data == ClickLabel::VarDesc)
    {
        Alia::showHelp(QUrl(QStringLiteral("http://envsupport.licor.com/help/EddyPro5/index.htm#Sensitive_and_Non-sensitive_Variables.htm")));
    }
    else if (data == ClickLabel::VarConv
             || data == ClickLabel::VarMinValue
             || data == ClickLabel::VarMaxValue)
    {
        Alia::showHelp(QUrl(QStringLiteral("http://envsupport.licor.com/help/EddyPro5/index.htm#Conversion_Type.htm")));
    }
    else if (data == ClickLabel::VarNomTLag
             || data == ClickLabel::VarMinTLag
             || data == ClickLabel::VarMaxTLag)
    {
        Alia::showHelp(QUrl(QStringLiteral("http://envsupport.licor.com/help/EddyPro5/index.htm#Nominal_Time_Lag.htm")));
    }
}

int CustomHeader::sectionCount()
{
    return layout->rowCount();
}
