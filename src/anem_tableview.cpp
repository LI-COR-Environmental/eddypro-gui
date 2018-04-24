/***************************************************************************
  anem_tableview.cpp
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

#include "anem_tableview.h"

#include <QHeaderView>
#include <QMouseEvent>
#include <QScrollBar>

#include "clicklabel.h"
#include "customheader.h"
#include "dbghelper.h"

AnemTableView::AnemTableView(QWidget *parent) :
    QTableView(parent)
{
    horizontalHeader()->show();
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

    m_header = new CustomHeader(this);
    m_header->addSection(tr("Manufacturer"), tr("<b>Manufacturer:</b> Specify the manufacturer of the anemometer among those supported. Choose <i>Other</i> for any manufacturer not explicitly listed. This field is mandatory."));
    m_header->addSection(tr("Model"), tr("<b>Model:</b> Identify the model of the anemometer. Choose <i>Generic Anemometer</i> for any model not explicitly listed. This field is mandatory."));
    m_header->addSection(tr("Embedded software version"), tr("<b>Embedded software version:</b> Identify the embedded software (firmware) version that was running on the selected anemometer. For Gill WindMaster and WindMaster Pro models, the firmware version is required in order to select the proper angle of attack correction. Storing other anemometers' firmware version is recommended for good recordkeeping."), CustomHeader::QuestionMarkHint::QuestionMark, ClickLabel::AnemSwVersion);
    m_header->addSection(tr("<i>Instrument ID</i>"), tr("<b>Instrument ID:</b> Enter an ID for the anemometer, to distinguish it from your other instruments. This is only for your records and providing it is optional."));
    m_header->addSection(tr("Height"), tr("<b> Height:</b> Enter the distance between the ground and the center of the device sampling volume. This field is mandatory."));
    m_header->addSection(tr("Wind data format"), tr("<b>Wind data format:</b> Specify the format in which the wind data are provided."));
    m_header->addSection(tr("North alignment"), tr("<b>North alignment:</b> Specify whether the anemometer\'s axes are aligned to transducers (<i>Axis</i>) or spars (<i>Spars</i>). For Gill anemometers only."), CustomHeader::QuestionMarkHint::QuestionMark, ClickLabel::AnemNAlign);
    m_header->addSection(tr("North off-set"), tr("<b>North offset:</b> Enter the anemometer\'s yaw offset with respect to local magnetic north (the one you assess with the compass), positive eastward."), CustomHeader::QuestionMarkHint::QuestionMark, ClickLabel::AnemNOffset);
    m_header->addSection(tr("Northward separation"), tr("<b>Northward separation:</b> Specify the distance between the current anemometer and the reference anemometer, as measured horizontally along the magnetic north-south axis (the one you assess with the compass). The distance is positive if the current anemometer is placed to the north of the reference anemometer. The reference anemometer is the first one you describe. For this anemometer you cannot enter the separation and you find the string Reference."), CustomHeader::QuestionMarkHint::QuestionMark, ClickLabel::AnemNSep);
    m_header->addSection(tr("Eastward separation"), tr("<b>Eastward separation:</b> Specify the distance between the current anemometer and reference anemometer, as measured horizontally along the east-west axis (the one you assess with the compass). The distance is positive if the current anemometer is placed to the east of the reference anemometer. The reference anemometer is the first one you describe. For this anemometer you cannot enter the separation and you find the string Reference."), CustomHeader::QuestionMarkHint::QuestionMark, ClickLabel::AnemESep);
    m_header->addSection(tr("Vertical separation"), tr("<b>Vertical separation:</b> Specify the distance between the current anemometer and the reference anemometer, as measured along the vertical axis. The distance is positive if the current anemometer is placed above the reference anemometer. The reference anemometer is the first one you describe. For this anemometer you cannot enter the separation and you find the string <i>Reference</i>."), CustomHeader::QuestionMarkHint::QuestionMark, ClickLabel::AnemVSep);
    m_header->addSection(tr("Longitudinal path length"), tr("<b>Longitudinal path length:</b> Path length in the direction defined by a pair of transducers. Consult the anemometer\'s specifications or user manual."));
    m_header->addSection(tr("Transversal path length"), tr("<b>Transversal path length:</b> Path length in the direction orthogonal to the longitudinal path length of the anemometer (e.g., as defined by the diameter of transducers)."));
    m_header->addSection(tr("Time response"), tr("<b>Time response:</b> Time response of the anemometer. Its inverse defines the maximum frequency of the atmospheric turbulence that the instrument is able to resolve. Consult the anemometer\'s specifications or user manual."));

    verticalHeader()->hide();
}

AnemTableView::~AnemTableView()
{
    delete m_header;
}

void AnemTableView::resizeEvent(QResizeEvent *event)
{
    setViewportMargins(m_header->sizeHint().width(), rowHeight(0) + 2, 0, 0);
    m_header->setGeometry(0,
                          static_cast<int>(rowHeight(0) / 2.0) + 2,
                          m_header->sizeHint().width() + 10,
                          rowHeight(0) * m_header->sectionCount());
    horizontalHeader()->setMinimumWidth(horizontalHeader()->count() * horizontalHeader()->sectionSize(1));
    horizontalScrollBar()->setMaximum((horizontalHeader()->count() - 1) * horizontalHeader()->sectionSize(1));
    horizontalScrollBar()->updateGeometry();
    viewport()->update();
    QWidget::resizeEvent(event);
}

void AnemTableView::showEvent(QShowEvent *event)
{
    setViewportMargins(m_header->sizeHint().width(), rowHeight(0) + 2, 0, 0);
    m_header->setGeometry(0,
                          static_cast<int>(rowHeight(0) / 2.0) + 2,
                          m_header->sizeHint().width() + 10,
                          rowHeight(0) * m_header->sectionCount());

    horizontalHeader()->setMinimumWidth(horizontalHeader()->count() * horizontalHeader()->sectionSize(1));
    horizontalScrollBar()->setMaximum((horizontalHeader()->count() - 1) * horizontalHeader()->sectionSize(1));
    horizontalScrollBar()->updateGeometry();
    viewport()->update();
    QWidget::showEvent(event);
}

void AnemTableView::mousePressEvent(QMouseEvent *event)
{
    QModelIndex item = indexAt(event->pos());
    if (!item.isValid())
    {
        reset();
    }

    QAbstractItemView::mousePressEvent(event);
}
