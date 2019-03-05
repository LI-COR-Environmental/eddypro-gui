/***************************************************************************
  variable_tableview.cpp
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

#include "variable_tableview.h"

#include <QDebug>
#include <QHeaderView>
#include <QMouseEvent>
#include <QScrollBar>

#include "customheader.h"

VariableTableView::VariableTableView(QWidget *parent) :
    QTableView(parent)
{
    auto hHeaderView = horizontalHeader();
    hHeaderView->show();
    connect(hHeaderView, &QHeaderView::sectionClicked,
            this, &VariableTableView::hHeaderClicked);

    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

    m_header = new CustomHeader(this);
    m_header->addSection(tr("Ignore"), tr("<b>Ignore:</b> Select <i>no</i> to tell EddyPro that a column (variable) is not purely numeric. Purely numeric variables are strings included within two consecutive field separators and containing only digits from 0 to 9 and, at most, the decimal dot. Any other character makes a variable a <i>non-numeric</i> one. For example, time stamps in the form of 2011-09-26 or times as 23:20:562 are not numeric variables. Note that if a variable is not numeric, this must be specified even if you set <i>yes</i> in the ignore field."), CustomHeader::QuestionMarkHint::QuestionMark, ClickLabel::VarIgnoreDesc);
    m_header->addSection(tr("Numeric"), tr("<b>Numeric:</b> Select 'no' to tell EddyPro that a column is not purely numeric. Purely numeric variables are strings included within two consecutive field separators and containing only digits from 0 to 9 and, at most, the decimal dot. Any other character makes a variable a not numeric one. For example, time stamps in the form of 2011-09-26 or times as 23:20:562 are not numeric variables. Note that if a variable is not numeric, this must be specified even if you set 'yes' in the ignore field."), CustomHeader::QuestionMarkHint::QuestionMark, ClickLabel::VarNumericDesc);
    m_header->addSection(tr("Variable"), tr("<b>Variable:</b> Specify the variable that is contained in the current column of the raw files (or position, for binary files). Purely numerical variables only contain numbers and the decimal dot (full stop)."), CustomHeader::QuestionMarkHint::QuestionMark, ClickLabel::VarDesc);
    m_header->addSection(tr("Instrument"), tr("<b>Instrument:</b> Select the instrument that measured the current variable. Instruments listed here are those entered under the instruments tab."));
    m_header->addSection(tr("Measurement type"), tr("<b>Measurement type:</b> Only applicable to gas concentrations. Enter the description of the concentration measurement (either <i>Molar/Mass density</i>, <i>Mole fraction</i>, or <i>Mixing ratio</i>). For all other variables, either leave the field blank or select <i>Other</i>. <i>Molar/Mass density</i> is a measure of mass per unit volume of air. <i>Mole fraction</i> is a measure of mass per mass of wet air. <i>Mixing ratio</i> is a measure of mass per mass of dry air. Measures of mass can be expressed as number of moles, grams, etc."));
    m_header->addSection(tr("Input unit"), tr("<b>Input unit:</b> Specify the units of the variable as it is stored in the raw file."));
    m_header->addSection(tr("Linear scaling"), tr("<b>Linear scaling:</b> Specify whether to perform a linear conversion to rescale data. Variables that are already in any of the supported physical units do not need to be rescaled."), CustomHeader::QuestionMarkHint::QuestionMark, ClickLabel::VarConv);
    m_header->addSection(tr("Output unit"), tr("<b>Output units:</b> Enter the output units (physical units after conversion). The following <b><i>Gain</i></b> and <b><i>Offset</i></b> values must be such that the input variable is converted into the selected output unit."));
    m_header->addSection(tr("Gain value"), tr("<b>Gain value:</b> Enter the gain (angular coefficient) of the linear relation between input and output units."));
    m_header->addSection(tr("Offset value"), tr("<b>Offset value:</b> Enter the offset (y-axis intercept) of the linear relation between input and output units."));
    m_header->addSection(tr("<i>Nominal time lag</i>"), tr("<b>Nominal time lag:</b> Enter the expected (nominal) time lag of the variable, with respect to the measurements of the anemometer that you plan to use for flux computation, as applicable. Time lags should be specified at least for gas concentrations and can be estimated based on instrument separation (open path) or on the sampling line characteristics and the flow rate (closed path)."), CustomHeader::QuestionMarkHint::QuestionMark, ClickLabel::VarNomTLag);
    m_header->addSection(tr("<i>Minimum time lag</i>"), tr("<b>Minimum time lag:</b> Enter the minimum expected time lag for the current variable, with respect to anemometric measurements."), CustomHeader::QuestionMarkHint::QuestionMark, ClickLabel::VarMinTLag);
    m_header->addSection(tr("<i>Maximum time lag</i>"), tr("<b>Maximum time lag:</b> Enter the maximum expected time lag for the current variable, with respect to anemometric measurements."), CustomHeader::QuestionMarkHint::QuestionMark, ClickLabel::VarMaxTLag);

    verticalHeader()->hide();
}

VariableTableView::~VariableTableView()
{
    delete m_header;
}

void VariableTableView::resizeEvent(QResizeEvent *event)
{
    setViewportMargins(m_header->sizeHint().width(), this->rowHeight(0) + 7, 1, 1);
    m_header->setGeometry(0,
                          static_cast<int>(this->rowHeight(0) / 2.0) + 8,
                          m_header->sizeHint().width() + 10,
                          this->rowHeight(0) * m_header->sectionCount());
    horizontalHeader()->setMinimumWidth(horizontalHeader()->count() * horizontalHeader()->sectionSize(1));
    horizontalScrollBar()->setMaximum((horizontalHeader()->count() - 1) * horizontalHeader()->sectionSize(1));
    horizontalScrollBar()->updateGeometry();
    viewport()->update();
    QWidget::resizeEvent(event);
}

void VariableTableView::showEvent(QShowEvent *event)
{
    setViewportMargins(m_header->sizeHint().width(), this->rowHeight(0) + 7, 1, 1);
    m_header->setGeometry(0,
                          static_cast<int>(this->rowHeight(0) / 2.0) + 8,
                          m_header->sizeHint().width() + 10,
                          this->rowHeight(0) * m_header->sectionCount());
    horizontalHeader()->setMinimumWidth(horizontalHeader()->count() * horizontalHeader()->sectionSize(1));
    horizontalScrollBar()->setMaximum((horizontalHeader()->count() - 1) * horizontalHeader()->sectionSize(1));
    horizontalScrollBar()->updateGeometry();
    viewport()->update();
    QWidget::showEvent(event);
}

void VariableTableView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        QModelIndex item = indexAt(event->pos());

        if (!item.isValid())
        {
            reset();
        }
    }

    QAbstractItemView::mousePressEvent(event);
}

// NOTE: to finish http://www.hardcoded.net/articles/how-to-customize-qtableview-editing-behavior
// http://stackoverflow.com/questions/12380107/in-qtableview-what-signal-triggers-the-editing-mode
void VariableTableView::closeEditor(QWidget *editor, QAbstractItemDelegate::EndEditHint hint)
{
    if (hint == QAbstractItemDelegate::NoHint)
    {
        QAbstractItemView::closeEditor(editor,
                                       QAbstractItemDelegate::SubmitModelCache);
    }

//    else if (hint == QAbstractItemDelegate::EditNextItem
//             || hint == QAbstractItemDelegate::EditPreviousItem)
//    {
//        int editableIndex;
//        if (hint == QAbstractItemDelegate::EditNextItem)
//        {
//            editableIndex = nextEditableIndex(currentIndex());
//        }
//        else
//        {
//            editableIndex = previousEditableIndex(currentIndex());
//        }

//        if (editableIndex == -1)
//        {
//            closeEditor(editor, QAbstractItemDelegate::SubmitModelCache);
//        }
//        else
//        {
//            closeEditor(editor, 0);
//            setCurrentIndex(editableIndex);
//            edit(editableIndex);
//        }
//    }

    else
    {
        QAbstractItemView::closeEditor(editor, hint);
    }
}

// NOTE: to finish
void VariableTableView::hHeaderClicked(int section)
{
    Q_UNUSED(section)
    clearSelection();
}

//void VariableTableView::firstEditableIndex(const QModelIndex& originalIndex,
//                                           columnIndexes)
//{

//}

// Returns the first editable index at the left of `originalIndex` or None.
void VariableTableView::previousEditableIndex(const QModelIndex& originalIndex)
{
    Q_UNUSED(originalIndex)
//    auto h = horizontalHeader();
//    auto myCol = originalIndex.column();
//    columnIndexes = [h.logicalIndex(i) for i in range(h.count())];

//    // keep only columns before myCol
//    columnIndexes = columnIndexes[:columnIndexes.index(myCol)];

//    // We want the previous item, the columns have to be in reverse order
//    columnIndexes = reversed(columnIndexes);

//    return firstEditableIndex(originalIndex, columnIndexes);
}

void VariableTableView::nextEditableIndex(const QModelIndex& originalIndex)
{
    Q_UNUSED(originalIndex)
}
