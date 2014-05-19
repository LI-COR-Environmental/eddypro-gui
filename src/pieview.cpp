/***************************************************************************
  pieview.cpp
  -------------------
  Copyright (C) 2012-2014, LI-COR Biosciences
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


#include <cmath>

#include <QDebug>
#include <QScrollBar>
#include <QMouseEvent>
#include <QPen>
#include <QPainter>
#include <QToolTip>

#ifndef M_PI
#define M_PI 3.1415927
#endif

#include "dbghelper.h"
#include "tablemodel.h"
#include "pieview.h"

PieView::PieView(QWidget *parent) : QAbstractItemView(parent)
{
    DEBUG_FUNC_NAME

    horizontalScrollBar()->setRange(0, 0);
    verticalScrollBar()->setRange(0, 0);

    margin = 26;
    totalSize = 200;
    pieSize = totalSize - 2 * margin;
    validItems = 0;
}

void PieView::dataChanged(const QModelIndex &topLeft,
                          const QModelIndex &bottomRight, const QVector<int> & roles)
{
    DEBUG_FUNC_NAME
    Q_UNUSED(roles)

    QAbstractItemView::dataChanged(topLeft, bottomRight);

    validItems = 0;
    for (int row = 0; row < model()->rowCount(rootIndex()); ++row)
    {
        QModelIndex index = model()->index(row, TableModel::DEGREES, rootIndex());
        double value = model()->data(index, Qt::EditRole).toDouble();

        if (value > 0.0)
        {
            validItems++;
        }
    }
    viewport()->update();
}

bool PieView::edit(const QModelIndex &/*index*/, EditTrigger /*trigger*/, QEvent */*event*/)
{
    return false;
}

/*
    Returns the item that covers the coordinate given in the view.
*/
QModelIndex PieView::indexAt(const QPoint &point) const
{
    DEBUG_FUNC_NAME
    if (validItems == 0)
        return QModelIndex();

    // Transform the view coordinates into contents widget coordinates.
    int wx = point.x() + horizontalScrollBar()->value();
    int wy = point.y() + verticalScrollBar()->value();

    if (wx < totalSize)
    {
        qDebug() << "wx" << wx << "totalSize" << totalSize;
        double cx = wx - totalSize/2;
        double cy = totalSize/2 - wy; // positive cy for items above the center
        qDebug() << "cx" << cx << "cy" << cy;

        // Determine the distance from the center point of the pie chart.
        double d = pow(pow(cx, 2) + pow(cy, 2), 0.5);
        qDebug() << "d" << d;

        if (d == 0 || d > pieSize/2)
        {
            return QModelIndex();
        }

        double offset = ((TableModel *)model())->offset();

        // determine the angle of the point in degrees starting from north
        double angle = (180 / M_PI) * asin(cx/d);
        qDebug() << "angle 1" << angle;

        if (cx > 0 && cy < 0)
            angle = 180 - angle;
        else if (cx < 0 && cy < 0)
            angle = 180 - angle;
        else if (cx < 0 && cy > 0)
            angle = 360 + angle;

        if ((angle - offset) < 0.0)
        {
            angle = 360.0 + angle;
            qDebug() << "angle 2" << angle;
        }

        angle = fmod(angle - offset, 360.0);
        qDebug() << "angle 3" << angle;

        // Find the relevant slice of the pie.
        // starting from north, clockwise
        double startAngle = 0.0;

        for (int row = 0; row < model()->rowCount(rootIndex()); ++row)
        {
            QModelIndex index = model()->index(row, TableModel::DEGREES, rootIndex());
            double value = model()->data(index, Qt::EditRole).toDouble();

            if (value > 0.0)
            {
                double sliceAngle = value;
                qDebug() << "sliceAngle" << sliceAngle;

                if (angle >= startAngle && angle <= (startAngle + sliceAngle))
                {
                    qDebug() << "found model()->index(row, TableModel::DEGREES, rootIndex())" << model()->index(row, TableModel::DEGREES, rootIndex()).row();
                    return model()->index(row, TableModel::DEGREES, rootIndex());
                }

                startAngle += sliceAngle;
                qDebug() << "startAngle" << startAngle;
            }
        }
    }
    else
    {
        double itemHeight = QFontMetrics(viewOptions().font).height();
        int listItem = int((wy - margin) / itemHeight);
        int validRow = 0;

        for (int row = 0; row < model()->rowCount(rootIndex()); ++row)
        {
            QModelIndex index = model()->index(row, TableModel::DEGREES, rootIndex());
            if (model()->data(index, Qt::EditRole).toDouble() > 0.0)
            {
                if (listItem == validRow)
                {
                    QModelIndex currIndex = model()->index(row, TableModel::DEGREES, rootIndex());
                    return currIndex;
                }

                // Update the list index that corresponds to the next valid row.
                validRow++;
            }
        }
    }

    return QModelIndex();
}

bool PieView::isIndexHidden(const QModelIndex & /*index*/) const
{
    return false;
}

/*
    Returns the rectangle of the item at position \a index in the
    model. The rectangle is in contents coordinates.
*/
QRect PieView::itemRect(const QModelIndex &index) const
{
    DEBUG_FUNC_NAME
    if (!index.isValid())
        return QRect();

    // Check whether the index's row is in the list of rows represented
    // by slices.
    QModelIndex valueIndex;
    if (index.column() != TableModel::DEGREES)
        valueIndex = model()->index(index.row(), TableModel::DEGREES, rootIndex());
    else
        valueIndex = index;

    qDebug() << "valueIndex row" << valueIndex.row();

    if (model()->data(valueIndex, Qt::EditRole).toDouble() > 0.0)
    {
        int listItem = 0;
        for (int row = index.row() - 1; row >= 0; --row)
        {
            if (model()->data(model()->index(row, TableModel::DEGREES, rootIndex()), Qt::EditRole).toDouble() > 0.0)
                listItem++;
        }

        switch (index.column())
        {
            case TableModel::DEGREES:
                return viewport()->rect();
        }
    }
    return QRect();
}

QRegion PieView::itemRegion(const QModelIndex &index) const
{
    DEBUG_FUNC_NAME
    if (!index.isValid())
        return QRegion();

    if (index.column() != TableModel::DEGREES)
        return itemRect(index);

    if (model()->data(index, Qt::EditRole).toDouble() < 0.0)
        return QRegion();

    double offset = ((TableModel *)model())->offset();

    // starting from north, clockwise
    double startAngle = 90.0 - offset;

    for (int row = 0; row < model()->rowCount(rootIndex()); ++row)
    {
        QModelIndex sliceIndex = model()->index(row, TableModel::DEGREES, rootIndex());
        double value = model()->data(sliceIndex, Qt::EditRole).toDouble();

        qDebug() << "index row" << index.row();
        qDebug() << "sliceIndex row" << sliceIndex.row();
        qDebug() << "value" << value;

        if (value > 0.0)
        {
            double angle = value;

            if (sliceIndex == index)
            {
                QPainterPath slicePath;

                slicePath.moveTo(totalSize/2, totalSize/2);
                slicePath.arcTo(margin, margin, margin + pieSize, margin + pieSize,
                                startAngle, -angle);

                slicePath.closeSubpath();

                qDebug() << QRegion(slicePath.toFillPolygon().toPolygon()).boundingRect();
                return QRegion(slicePath.toFillPolygon().toPolygon());
            }
            startAngle -= angle;
        }
    }
    return QRegion();
}

int PieView::horizontalOffset() const
{
    return horizontalScrollBar()->value();
}

void PieView::mousePressEvent(QMouseEvent *event)
{
    DEBUG_FUNC_NAME
    QModelIndex item = indexAt(event->pos());
    if (!item.isValid())
        clearSelection();

    QAbstractItemView::mousePressEvent(event);
}

void PieView::mouseDoubleClickEvent(QMouseEvent *event)
{
    DEBUG_FUNC_NAME
    QModelIndex item = indexAt(event->pos());
    if (!item.isValid())
    {
        emit fillPieRequest();
        return;
    }

    QAbstractItemView::mouseDoubleClickEvent(event);
}

void PieView::mouseMoveEvent(QMouseEvent *event)
{
    QAbstractItemView::mouseMoveEvent(event);
}

void PieView::mouseReleaseEvent(QMouseEvent *event)
{
    QAbstractItemView::mouseReleaseEvent(event);
}

QModelIndex PieView::moveCursor(QAbstractItemView::CursorAction cursorAction,
                                Qt::KeyboardModifiers /*modifiers*/)
{
    QModelIndex current = currentIndex();
    int row = current.row();
    int column = current.column();

    switch (cursorAction)
    {
        case MoveLeft:
        case MoveUp:
            if (current.row() > 0)
            {
                current = model()->index(row - 1, column, rootIndex());
            }
            else
            {
                current = model()->index(0, current.column(), rootIndex());
            }
            break;
        case MoveRight:
        case MoveDown:
            if (current.row() < rows(current) - 1)
            {
                current = model()->index(row + 1, column, rootIndex());
            }
            else
            {
                current = model()->index(rows(current) - 1, column, rootIndex());
            }
            break;
        default:
            break;
    }

    viewport()->update();
    return current;
}

void PieView::paintEvent(QPaintEvent *event)
{
    DEBUG_FUNC_NAME
    QItemSelectionModel *selections = selectionModel();

    qDebug() << "selections" << selections->currentIndex().row();

    QStyleOptionViewItem option = viewOptions();

    QBrush background = option.palette.base();
    QPen foreground(Qt::gray);
    foreground.setWidthF(0.3);

    QPainter painter(viewport());
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(event->rect(), background);
    painter.setPen(foreground);

    // Viewport rectangles
    QRect pieRect = QRect(margin, margin, pieSize, pieSize);

    double offset = ((TableModel *)model())->offset();

    painter.save();

    QLinearGradient fade(0, 0, 0, height());
    fade.setColorAt(0, QColor("#d4f3ff"));
    fade.setColorAt(1, QColor("#aae6ff"));
    // NOTE: to verify
    QFont myFont(QStringLiteral("Open Sans"), 14, QFont::Bold);
    QPainterPath myPath;
    myPath.addText(QPointF(94, 20), myFont, tr("N"));
    myPath.addText(QPointF(178, 105), myFont, tr("E"));
    myPath.addText(QPointF(94, 192), myFont, tr("S"));
    myPath.addText(QPointF(4, 105), myFont, tr("W"));
    myPath.closeSubpath();
    painter.setBrush(fade);
    painter.setPen(QPen(Qt::darkCyan));
    painter.drawPath(myPath);

    painter.restore();

    qDebug() << "validItems" << validItems;
    if (validItems >= 0)
    {
        painter.save();
        painter.translate(pieRect.x() - horizontalScrollBar()->value(),
                          pieRect.y() - verticalScrollBar()->value());
        painter.drawEllipse(0, 0, pieSize, pieSize);

        // starting from north, clockwise
        double startAngle = 90.0 - offset;
        int row;

        for (row = 0; row < model()->rowCount(rootIndex()); ++row)
        {
            QModelIndex index = model()->index(row, TableModel::DEGREES, rootIndex());
            double value = model()->data(index, Qt::EditRole).toDouble();

            qDebug() << "row" << row;

            if (value > 0.0)
            {
                double angle = value;

                QModelIndex colorIndex = model()->index(row, TableModel::DEGREES, rootIndex());
                QColor color = QColor(model()->data(colorIndex,
                                Qt::DecorationRole).toString());

                // pie rect without margins
                QRect pieRect2(0, 0, pieSize, pieSize);

                QColor satColor(color);
                satColor.setHsv(color.hue(), 255, 230);

                // gradient for the pie pieces
                QRadialGradient rg(pieRect2.center(),
                                   pieRect2.width() / 2 - 1,
                                   pieRect2.topLeft());
                rg.setColorAt(0, Qt::white);
                rg.setColorAt(1, color);

                QBrush selectionPattern(satColor);

                int checked = model()->data(colorIndex, Qt::CheckStateRole).toInt();

                if (currentIndex() == index && selections->hasSelection())
                {
                    if (checked)
                    {
                        qDebug() << "satColor";
                        painter.setBrush(selectionPattern);
                    }
                    else
                    {
                        painter.setBrush(QBrush(Qt::darkGray));
                    }
                }
                else if (selections->isSelected(index))
                {
                    qDebug() << "color Dense2Pattern";
                    qDebug() << "current " << currentIndex();
                    qDebug() << "selected " << selections->selectedIndexes();

                    // NOTE: hack for bad setSelection() results
                    selections->setCurrentIndex(currentIndex(), QItemSelectionModel::ClearAndSelect);
                    qDebug() << "selected " << selections->selectedIndexes();
                }
                else
                {
                    if (checked)
                    {
                        qDebug() << "color";
                        painter.setBrush(rg);
                    }
                    else
                    {
                        painter.setBrush(QBrush(Qt::lightGray));
                    }
                }

                // clockwise
                painter.drawPie(0, 0, pieSize, pieSize,
                                int((startAngle)*16),
                                int(-angle*16));
                startAngle -= angle;
            }
        }
        painter.restore();
    }
}

QSize PieView::sizeHint() const
{
    return QSize(200, 200);
}

void PieView::resizeEvent(QResizeEvent * /* event */)
{
    updateGeometries();
}

int PieView::rows(const QModelIndex &index) const
{
    return model()->rowCount(model()->parent(index));
}

void PieView::rowsInserted(const QModelIndex &parent, int start, int end)
{
    DEBUG_FUNC_NAME
    for (int row = start; row <= end; ++row)
    {
        QModelIndex index = model()->index(row, TableModel::DEGREES, rootIndex());
        double value = model()->data(index, Qt::EditRole).toDouble();

        if (value > 0.0)
        {
            validItems++;
        }
    }

    QAbstractItemView::rowsInserted(parent, start, end);
}

void PieView::rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
    DEBUG_FUNC_NAME
    for (int row = start; row <= end; ++row)
    {
        QModelIndex index = model()->index(row, TableModel::DEGREES, rootIndex());
        double value = model()->data(index, Qt::EditRole).toDouble();
        if (value > 0.0)
        {
            validItems--;
        }
    }

    QAbstractItemView::rowsAboutToBeRemoved(parent, start, end);
}

void PieView::scrollContentsBy(int dx, int dy)
{
    viewport()->scroll(dx, dy);
}

void PieView::scrollTo(const QModelIndex &index, ScrollHint)
{
    QRect area = viewport()->rect();
    QRect rect = visualRect(index);

    if (rect.left() < area.left())
        horizontalScrollBar()->setValue(
            horizontalScrollBar()->value() + rect.left() - area.left());
    else if (rect.right() > area.right())
        horizontalScrollBar()->setValue(
            horizontalScrollBar()->value() + qMin(
                rect.right() - area.right(), rect.left() - area.left()));

    if (rect.top() < area.top())
        verticalScrollBar()->setValue(
            verticalScrollBar()->value() + rect.top() - area.top());
    else if (rect.bottom() > area.bottom())
        verticalScrollBar()->setValue(
            verticalScrollBar()->value() + qMin(
                rect.bottom() - area.bottom(), rect.top() - area.top()));

    update();
}

/*
    Find the indices corresponding to the extent of the selection.
*/
void PieView::setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command)
{
    DEBUG_FUNC_NAME
    // Use content widget coordinates because we will use the itemRegion()
    // function to check for intersections.
    QRect contentsRect = rect.translated(
                            horizontalScrollBar()->value(),
                            verticalScrollBar()->value()).normalized();
    qDebug() << "command flags" << command;
    qDebug() << "contentsRect" << contentsRect;

    int rows = model()->rowCount(rootIndex());
    int columns = model()->columnCount(rootIndex());
    qDebug() << "rows" << rows;
    qDebug() << "columns" << columns;
    QModelIndexList indexes;

    for (int row = 0; row < rows; ++row)
    {
        for (int column = 0; column < columns; ++column)
        {
            qDebug() << "row, col:" << row << column;
            QModelIndex index = model()->index(row, column, rootIndex());
            QRegion region = itemRegion(index);

            if (!region.intersected(contentsRect).isEmpty())
            {
                qDebug() << "intersection!";
                indexes.append(index);
            }
            else
            {
                qDebug() << "no intersection";
            }
        }
    }

    if (indexes.size() > 0)
    {
        qDebug() << "indexes.size() > 0" << indexes.size();
        int firstRow = indexes[0].row();
        int lastRow = indexes[0].row();
        int firstColumn = indexes[0].column();
        int lastColumn = indexes[0].column();

        for (int i = 1; i < indexes.size(); ++i)
        {
            firstRow = qMin(firstRow, indexes[i].row());
            lastRow = qMax(lastRow, indexes[i].row());
            firstColumn = qMin(firstColumn, indexes[i].column());
            lastColumn = qMax(lastColumn, indexes[i].column());
        }

        QItemSelection selection(
            model()->index(firstRow, firstColumn, rootIndex()),
            model()->index(lastRow, lastColumn, rootIndex()));
        selectionModel()->select(selection, command);
        qDebug() << "selection" << selection;
    }
    else
    {
        qDebug() << "indexes.size() < 0" << indexes.size();
        QModelIndex noIndex;
        QItemSelection selection(noIndex, noIndex);
        selectionModel()->select(selection, command);
    }

    update();
}

void PieView::updateGeometries()
{
    horizontalScrollBar()->setPageStep(viewport()->width());
    horizontalScrollBar()->setRange(0, qMax(0, 2*totalSize - viewport()->width()));
    verticalScrollBar()->setPageStep(viewport()->height());
    verticalScrollBar()->setRange(0, qMax(0, totalSize - viewport()->height()));
}

int PieView::verticalOffset() const
{
    return verticalScrollBar()->value();
}

/*
    Returns the position of the item in viewport coordinates.
*/
QRect PieView::visualRect(const QModelIndex &index) const
{
    DEBUG_FUNC_NAME
    QRect rect = itemRect(index);
    if (rect.isValid())
        return QRect(rect.left() - horizontalScrollBar()->value(),
                     rect.top() - verticalScrollBar()->value(),
                     rect.width(), rect.height());
    else
        return rect;
}

/*
    Returns a region corresponding to the selection in viewport coordinates.
*/
QRegion PieView::visualRegionForSelection(const QItemSelection &selection) const
{
    DEBUG_FUNC_NAME
    int ranges = selection.count();

    if (ranges == 0)
        return QRect();

    QRegion region;
    for (int i = 0; i < ranges; ++i)
    {
        QItemSelectionRange range = selection.at(i);
        for (int row = range.top(); row <= range.bottom(); ++row)
        {
            for (int col = range.left(); col <= range.right(); ++col)
            {
                QModelIndex index = model()->index(row, col, rootIndex());
                region += visualRect(index);
            }
        }
    }
    return region;
}

void PieView::updateValidItems()
{
    validItems = model()->rowCount();
}

bool PieView::viewportEvent(QEvent *event)
{
   if (event->type() == QEvent::ToolTip)
   {
       QHelpEvent *helpEvent = static_cast<QHelpEvent*>(event);
         QToolTip::showText(helpEvent->globalPos(), this->toolTip());
   }
   return QAbstractItemView::viewportEvent(event);
}
