/***************************************************************************
  irga_tableview.cpp
  ------------------
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

#include "irga_tableview.h"

#include <QHeaderView>
#include <QMouseEvent>
#include <QScrollBar>

#include "clicklabel.h"
#include "customheader.h"
#include "defs.h"

IrgaTableView::IrgaTableView(QWidget *parent) :
    QTableView(parent)
{
    horizontalHeader()->show();
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

    m_header = new CustomHeader(this);
    m_header->addSection(tr("Manufacturer"), tr("<b>Manufacturer:</b> Specify the manufacturer of the gas analyzer. For gas analyzers other than LI-COR, select <i>Other</i>. This field is mandatory."));
    m_header->addSection(tr("Model"), tr("<b>Model:</b> Identify the model of the gas analyzer. For gas analyzers other than LI-COR, select the appropriate typology. OP and CP stand for open path and closed path, respectively."), CustomHeader::QuestionMarkHint::QuestionMark, ClickLabel::IrgaModel);
    m_header->addSection(tr("<i>Embedded software version</i>"), tr("<b>Embedded software version:</b> Identify the embedded software version that was running on the LI-7550 Analyzer Interface Unit at the time data were collected."));
    m_header->addSection(tr("<i>Instrument ID</i>"), tr("<b>Instrument ID:</b> Enter an ID for the gas analyzer, to distinguish it from your other instruments. This is for your records and providing it is optional."));
    m_header->addSection(tr("Tube length"), tr("<b>Tube length:</b> Specify the length of the sample intake tube in centimeters. This field is mandatory for closed path instruments. Ignore it for open path instruments."));
    m_header->addSection(tr("Tube inner diameter"), tr("<b>Tube inner diameter:</b> Specify the inside diameter of the intake tube in centimeters. This field is mandatory for closed path instruments. Ignore it for open path instruments."));
    m_header->addSection(tr("Nominal tube flow rate"), tr("<b>Nominal tube flow rate:</b> Specify the flow rate in the intake tube expected during normal operation. This field is mandatory for closed path instruments. Ignore it for open path instruments."));
    m_header->addSection(tr("Northward separation"), tr("<b>Northward separation:</b> Specify the distance between the center of the sample volume (or the inlet of the intake tube) of the current gas analyzer and the reference anemometer, as measured horizontally along the north-south axis (the one you assess with the compass). The distance is positive if the gas analyzer is placed to the north of the anemometer."));
    m_header->addSection(tr("Eastward separation"), tr("<b>Eastward separation:</b> Specify the distance between the center of the sample volume (or the inlet of the intake tube) of the current gas analyzer and the reference anemometer, as measured horizontally along the east-west axis (the one you assess with the compass). The distance is positive if the gas analyzer is placed to the east of the anemometer."));
    m_header->addSection(tr("Vertical separation"), tr("<b>Vertical separation:</b> Specify the distance between the center of the sample volume (or the inlet of the intake tube) of the current gas analyzer and the reference anemometer, as measured along the vertical axis. The distance is positive if the gas analyzer is above the anemometer."));
    m_header->addSection(tr("Longitudinal path length"), tr("<b>Longitudinal path length:</b> Path length in the direction of the optical source. Consult the analyzer's specifications or user manual."), CustomHeader::QuestionMarkHint::QuestionMark, ClickLabel::IrgaLPathLength);
    m_header->addSection(tr("Transversal path length"), tr("<b>Transversal path length:</b> Path length in the direction orthogonal to the optical source. Consult the analyzer's specifications or user manual."), CustomHeader::QuestionMarkHint::QuestionMark, ClickLabel::IrgaTPathLength);
    m_header->addSection(tr("Time response"), tr("<b>Time response:</b> Time response of the gas analyzer. Its inverse defines the maximum frequency of the atmospheric turbulent concentration fluctuations that the instrument is able to resolve. Consult the analyzer's specifications or user manual."));
    m_header->addSection(tr("Extinction coeff in Water, k<sub>W</sub>"), tr("<b>Extinction coefficient in Water, k<sub>W</sub>:</b> in Krypton or Lyman-%1 hygrometers, the extinction coefficients for oxygen of the hygrometers, associated with the third-order Taylor expansion of the Lambert-Beer law around reference conditions (van Dijk et al. 2003).").arg(Defs::ALPHA));
    m_header->addSection(tr("Extinction coeff in Oxygen, k<sub>O</sub>"), tr("<b>Extinction coefficient in Oxygen, k<sub>O</sub>:</b> in Krypton or Lyman-%1 hygrometers, the extinction coefficients for oxygen of the hygrometers, associated with the third-order Taylor expansion of the Lambert-Beer law around reference conditions (van Dijk et al. 2003).").arg(Defs::ALPHA));

    verticalHeader()->hide();
}

IrgaTableView::~IrgaTableView()
{
    delete m_header;
}

void IrgaTableView::resizeEvent(QResizeEvent *event)
{
    setViewportMargins(m_header->sizeHint().width() + 40, rowHeight(0) + 7, 1, 1);
    m_header->setGeometry(0,
                          static_cast<int>(rowHeight(0) / 2.0) + 8,
                          m_header->sizeHint().width() + 10,
                          rowHeight(0) * m_header->sectionCount());
    horizontalHeader()->setMinimumWidth(horizontalHeader()->count() * horizontalHeader()->sectionSize(1));
    horizontalScrollBar()->setMaximum((horizontalHeader()->count() - 1) * horizontalHeader()->sectionSize(1));
    horizontalScrollBar()->updateGeometry();
    viewport()->update();
    QWidget::resizeEvent(event);
}

void IrgaTableView::showEvent(QShowEvent *event)
{
    setViewportMargins(m_header->sizeHint().width() + 40, rowHeight(0) + 7, 1, 1);
    m_header->setGeometry(0,
                          static_cast<int>(rowHeight(0) / 2.0) + 8,
                          m_header->sizeHint().width() + 10,
                          rowHeight(0) * m_header->sectionCount());
    horizontalHeader()->setMinimumWidth(horizontalHeader()->count() * horizontalHeader()->sectionSize(1));
    horizontalScrollBar()->setMaximum((horizontalHeader()->count() - 1) * horizontalHeader()->sectionSize(1));
    horizontalScrollBar()->updateGeometry();
    viewport()->update();
    QWidget::showEvent(event);
}

void IrgaTableView::mousePressEvent(QMouseEvent *event)
{
    QModelIndex item = indexAt(event->pos());
    if (!item.isValid())
    {
        reset();
    }

    QAbstractItemView::mousePressEvent(event);
}
