/***************************************************************************
  angles_view.cpp
  -------------------
  Copyright (C) 2012-2018, LI-COR Biosciences
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

#include "angles_view.h"

#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QScrollBar>
#include <QToolTip>

#include <cmath>

#ifndef M_PI
#define M_PI 3.1415927
#endif

#include "angle_tablemodel.h"

AnglesView::AnglesView(QWidget *parent) : QAbstractItemView(parent)
{
    horizontalScrollBar()->setRange(0, 0);
    verticalScrollBar()->setRange(0, 0);

    margin = 26;
    totalSize = 200;
    pieSize = totalSize - 2 * margin;
    validItems = 0;
}

void AnglesView::dataChanged(const QModelIndex &topLeft,
                          const QModelIndex &bottomRight, const QVector<int> & roles)
{
    // NOTE: see if the new roles var is needed to updated the pie
    Q_UNUSED(roles)

    QAbstractItemView::dataChanged(topLeft, bottomRight);

    validItems = 0;
    for (auto row = 0; row < model()->rowCount(rootIndex()); ++row)
    {
        auto index = model()->index(row, AngleTableModel::DEGREES, rootIndex());
        auto value = model()->data(index, Qt::EditRole).toDouble();

        if (value > 0.0)
        {
            validItems++;
        }
    }
    viewport()->update();
}

bool AnglesView::edit(const QModelIndex &/*index*/, EditTrigger /*trigger*/, QEvent */*event*/)
{
    return false;
}

/*
    Returns the item that covers the coordinate given in the view.
*/
QModelIndex AnglesView::indexAt(const QPoint &point) const
{
    if (validItems == 0)
        return QModelIndex{};

    // Transform the view coordinates into contents widget coordinates.
    auto wx = point.x() + horizontalScrollBar()->value();
    auto wy = point.y() + verticalScrollBar()->value();

    if (wx < totalSize)
    {
        auto cx = wx - totalSize / 2;
        auto cy = totalSize / 2 - wy; // positive cy for items above the center

        // Determine the distance from the center point of the pie chart.
        auto d = pow(pow(cx, 2) + pow(cy, 2), 0.5);

        if (qFuzzyCompare(d + 1, 0.0 + 1) || (d > pieSize / 2.0))
        {
            return QModelIndex{};
        }

        auto offset = dynamic_cast<AngleTableModel *>(model())->offset();

        // determine the angle of the point in degrees starting from north
        auto angle = (180.0 / M_PI) * asin(cx/d);

        if (cx > 0.0 && cy < 0.0)
            angle = 180.0 - angle;
        else if (cx < 0.0 && cy < 0.0)
            angle = 180.0 - angle;
        else if (cx < 0.0 && cy > 0.0)
            angle = 36.00 + angle;

        if ((angle - offset) < 0.0)
        {
            angle = 360.0 + angle;
        }

        angle = fmod(angle - offset, 360.0);

        // Find the relevant slice of the pie.
        // starting from north, clockwise
        auto startAngle = 0.0;

        for (auto row = 0; row < model()->rowCount(rootIndex()); ++row)
        {
            auto index = model()->index(row, AngleTableModel::DEGREES, rootIndex());
            auto value = model()->data(index, Qt::EditRole).toDouble();

            if (value > 0.0)
            {
                auto sliceAngle = value;

                if (angle >= startAngle && angle <= (startAngle + sliceAngle))
                {
                    return model()->index(row, AngleTableModel::DEGREES, rootIndex());
                }

                startAngle += sliceAngle;
            }
        }
    }
    else
    {
        auto itemHeight = QFontMetrics(viewOptions().font).height();

        double listItemDouble;
        modf((wy - margin) / (itemHeight + 0.0), &listItemDouble);
        auto listItem = static_cast<int>(listItemDouble);

        auto validRow = 0;

        for (auto row = 0; row < model()->rowCount(rootIndex()); ++row)
        {
            auto index = model()->index(row, AngleTableModel::DEGREES, rootIndex());
            if (model()->data(index, Qt::EditRole).toDouble() > 0.0)
            {
                if (listItem == validRow)
                {
                    QModelIndex currIndex = model()->index(row, AngleTableModel::DEGREES, rootIndex());
                    return currIndex;
                }

                // Update the list index that corresponds to the next valid row.
                validRow++;
            }
        }
    }

    return QModelIndex{};
}

bool AnglesView::isIndexHidden(const QModelIndex & /*index*/) const
{
    return false;
}

/*
    Returns the rectangle of the item at position \a index in the
    model. The rectangle is in contents coordinates.
*/
QRect AnglesView::itemRect(const QModelIndex &index) const
{
    if (!index.isValid())
        return QRect{};

    // Check whether the index's row is in the list of rows represented
    // by slices.
    QModelIndex valueIndex;
    if (index.column() != AngleTableModel::DEGREES)
        valueIndex = model()->index(index.row(), AngleTableModel::DEGREES, rootIndex());
    else
        valueIndex = index;

    if (model()->data(valueIndex, Qt::EditRole).toDouble() > 0.0)
    {
        auto listItem = 0;
        for (auto row = index.row() - 1; row >= 0; --row)
        {
            if (model()->data(model()->index(row, AngleTableModel::DEGREES, rootIndex()), Qt::EditRole).toDouble() > 0.0)
                listItem++;
        }

        switch (index.column())
        {
            case AngleTableModel::DEGREES:
                return viewport()->rect();
        }
    }
    return QRect{};
}

QRegion AnglesView::itemRegion(const QModelIndex &index) const
{
    if (!index.isValid())
        return QRegion();

    if (index.column() != AngleTableModel::DEGREES)
        return itemRect(index);

    if (model()->data(index, Qt::EditRole).toDouble() < 0.0)
        return QRegion();

    auto offset = dynamic_cast<AngleTableModel *>(model())->offset();

    // starting from north, clockwise
    auto startAngle = 90.0 - offset;

    for (auto row = 0; row < model()->rowCount(rootIndex()); ++row)
    {
        auto sliceIndex = model()->index(row, AngleTableModel::DEGREES, rootIndex());
        auto value = model()->data(sliceIndex, Qt::EditRole).toDouble();

        if (value > 0.0)
        {
            auto spanAngle = value;

            if (sliceIndex == index)
            {
                QPainterPath slicePath;

                slicePath.moveTo(totalSize / 2.0, totalSize / 2.0);
                slicePath.arcTo(margin, margin, margin + pieSize, margin + pieSize,
                                startAngle, -spanAngle);

//                // Viewport rectangles
//                QRect pieRect = QRect(margin, margin, pieSize, pieSize);
//                slicePath.translate(pieRect.x() - horizontalScrollBar()->value(),
//                                  pieRect.y() - verticalScrollBar()->value());
//                slicePath.arcTo(0, 0, pieSize, pieSize,
//                                startAngle, -angle);

                slicePath.closeSubpath();

                return QRegion(slicePath.toFillPolygon().toPolygon());
            }
            startAngle -= spanAngle;
        }
    }
    return QRegion();
}

int AnglesView::horizontalOffset() const
{
    return horizontalScrollBar()->value();
}

void AnglesView::mousePressEvent(QMouseEvent *event)
{
    auto item = indexAt(event->pos());
    if (!item.isValid())
        clearSelection();

    QAbstractItemView::mousePressEvent(event);
}

void AnglesView::mouseDoubleClickEvent(QMouseEvent *event)
{
    auto item = indexAt(event->pos());
    if (!item.isValid())
    {
        emit fillPieRequest();
        return;
    }

    QAbstractItemView::mouseDoubleClickEvent(event);
}

void AnglesView::mouseMoveEvent(QMouseEvent *event)
{
    QAbstractItemView::mouseMoveEvent(event);
}

void AnglesView::mouseReleaseEvent(QMouseEvent *event)
{
    QAbstractItemView::mouseReleaseEvent(event);
}

QModelIndex AnglesView::moveCursor(QAbstractItemView::CursorAction cursorAction,
                                Qt::KeyboardModifiers /*modifiers*/)
{
    auto current = currentIndex();
    auto row = current.row();
    auto column = current.column();

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
        case MoveHome:
        case MoveEnd:
        case MovePageUp:
        case MovePageDown:
        case MoveNext:
        case MovePrevious:
        default:
            break;
    }

    viewport()->update();
    return current;
}

void AnglesView::paintEvent(QPaintEvent *event)
{
    auto selections = selectionModel();
    auto option = viewOptions();

    auto background = option.palette.base();
    QPen foreground(Qt::gray);
    foreground.setWidthF(0.3);

    QPainter painter(viewport());
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(event->rect(), background);
    painter.setPen(foreground);

    // Viewport rectangles
    auto pieRect = QRect(margin, margin, pieSize, pieSize);

    auto offset = dynamic_cast<AngleTableModel *>(model())->offset();

    painter.save();

    QLinearGradient fade(0, 0, 0, height());
    fade.setColorAt(0, QColor(212, 243, 255)); // #d4f3ff"
    fade.setColorAt(1, QColor(170, 230, 255)); // #aae6ff
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

    // for debugging
//    QPen textPen(option.palette.color(QPalette::Text));
//    painter.setPen(textPen);
//    painter.drawText(QPointF(20, 20), QStringLiteral("valid items: ") + QString::number(validItems));

    painter.restore();

    if (validItems >= 0)
    {
        painter.save();
        painter.translate(pieRect.x() - horizontalScrollBar()->value(),
                          pieRect.y() - verticalScrollBar()->value());
        painter.drawEllipse(0, 0, pieSize, pieSize);

        // starting from north, clockwise
        double startAngle = 90.0 - offset;
        for (auto row = 0; row < model()->rowCount(rootIndex()); ++row)
        {
            auto index = model()->index(row, AngleTableModel::DEGREES, rootIndex());
            auto value = model()->data(index, Qt::EditRole).toDouble();

            if (value > 0.0)
            {
                auto spanAngle = value;

                auto colorIndex = model()->index(row, AngleTableModel::DEGREES, rootIndex());
                auto color = QColor(model()->data(colorIndex,
                                Qt::DecorationRole).toString());

                // pie rect without margins
                QRect pieRect2(0, 0, pieSize, pieSize);

                QColor satColor(color);
                satColor.setHsv(color.hue(), 255, 230);

                // gradient for the pie pieces
                QRadialGradient rg(pieRect2.center(),
                                   pieRect2.width() / 2.0 - 1.0,
                                   pieRect2.topLeft());
                rg.setColorAt(0, Qt::white);
                rg.setColorAt(1, color);

                QBrush selectionPattern(satColor);

                auto checked = model()->data(colorIndex, Qt::CheckStateRole).toInt();

                if (currentIndex() == index && selections->hasSelection())
                {
                    if (checked)
                    {
                        painter.setBrush(selectionPattern);
                    }
                    else
                    {
                        painter.setBrush(QBrush(Qt::darkGray));
                    }
                }
                else if (selections->isSelected(index))
                {
                    // NOTE: hack for bad setSelection() results
                    selections->setCurrentIndex(currentIndex(), QItemSelectionModel::ClearAndSelect);
                }
                else
                {
                    if (checked)
                    {
                        painter.setBrush(rg);
                    }
                    else
                    {
                        painter.setBrush(QBrush(Qt::lightGray));
                    }
                }

                // clockwise
                double int_part;

                modf(startAngle * 16.0, &int_part);
                auto stAngle = static_cast<int>(int_part);

                modf(-spanAngle * 16.0, &int_part);
                auto spAngle = static_cast<int>(int_part);

                painter.drawPie(0, 0, pieSize, pieSize, stAngle, spAngle);
                startAngle -= spanAngle;
            }
        }
        painter.restore();
    }
}

QSize AnglesView::sizeHint() const
{
    return QSize{200, 200};
}

void AnglesView::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    updateGeometries();
}

int AnglesView::rows(const QModelIndex &index) const
{
    return model()->rowCount(model()->parent(index));
}

void AnglesView::rowsInserted(const QModelIndex &parent, int start, int end)
{
    for (auto row = start; row <= end; ++row)
    {
        auto index = model()->index(row, AngleTableModel::DEGREES, rootIndex());
        auto value = model()->data(index, Qt::EditRole).toDouble();

        if (value > 0.0)
        {
            validItems++;
        }
    }

    QAbstractItemView::rowsInserted(parent, start, end);
}

void AnglesView::rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
    for (auto row = start; row <= end; ++row)
    {
        auto index = model()->index(row, AngleTableModel::DEGREES, rootIndex());
        auto value = model()->data(index, Qt::EditRole).toDouble();
        if (value > 0.0)
        {
            validItems--;
        }
    }

    QAbstractItemView::rowsAboutToBeRemoved(parent, start, end);
}

void AnglesView::scrollContentsBy(int dx, int dy)
{
    viewport()->scroll(dx, dy);
}

void AnglesView::scrollTo(const QModelIndex &index, ScrollHint)
{
    auto area = viewport()->rect();
    auto rect = visualRect(index);

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
void AnglesView::setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command)
{
    // Use content widget coordinates because we will use the itemRegion()
    // function to check for intersections.
    auto contentsRect = rect.translated(
                            horizontalScrollBar()->value(),
                            verticalScrollBar()->value()).normalized();

    auto rows = model()->rowCount(rootIndex());
    auto columns = model()->columnCount(rootIndex());
    QModelIndexList indexes;

    for (auto row = 0; row < rows; ++row)
    {
        for (auto column = 0; column < columns; ++column)
        {
            auto index = model()->index(row, column, rootIndex());
            auto region = itemRegion(index);

            if (!region.intersected(contentsRect).isEmpty())
            {
                indexes.append(index);
            }
            else
            {
                qDebug() << "no intersection";
            }
        }
    }

    if (!indexes.isEmpty())
    {
        auto firstRow = indexes[0].row();
        auto lastRow = indexes[0].row();
        auto firstColumn = indexes[0].column();
        auto lastColumn = indexes[0].column();

        for (auto i = 1; i < indexes.size(); ++i)
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
    }
    else
    {
        QModelIndex noIndex;
        QItemSelection selection(noIndex, noIndex);
        selectionModel()->select(selection, command);
    }

    update();
}

void AnglesView::updateGeometries()
{
    horizontalScrollBar()->setPageStep(viewport()->width());
    horizontalScrollBar()->setRange(0, qMax(0, 2*totalSize - viewport()->width()));
    verticalScrollBar()->setPageStep(viewport()->height());
    verticalScrollBar()->setRange(0, qMax(0, totalSize - viewport()->height()));
}

int AnglesView::verticalOffset() const
{
    return verticalScrollBar()->value();
}

/*
    Returns the position of the item in viewport coordinates.
*/
QRect AnglesView::visualRect(const QModelIndex &index) const
{
    auto rect = itemRect(index);
    if (rect.isValid())
        return QRect{rect.left() - horizontalScrollBar()->value(),
                     rect.top() - verticalScrollBar()->value(),
                     rect.width(), rect.height()};
    return rect;
}

/*
    Returns a region corresponding to the selection in viewport coordinates.
*/
QRegion AnglesView::visualRegionForSelection(const QItemSelection &selection) const
{
    auto ranges = selection.count();

    if (ranges == 0)
        return QRect();

    QRegion region;
    for (auto i = 0; i < ranges; ++i)
    {
        const auto& range = selection.at(i);
        for (auto row = range.top(); row <= range.bottom(); ++row)
        {
            for (auto col = range.left(); col <= range.right(); ++col)
            {
                QModelIndex index = model()->index(row, col, rootIndex());
                region += visualRect(index);
            }
        }
    }
    return region;
}

void AnglesView::updateValidItems()
{
    validItems = model()->rowCount();
}

bool AnglesView::viewportEvent(QEvent *event)
{
   if (event->type() == QEvent::ToolTip)
   {
       auto helpEvent = dynamic_cast<QHelpEvent*>(event);
//       QModelIndex index = indexAt(he->pos());
//       QRect vr = visualRect(index);
//       QRect shr = itemDelegate(index)->sizeHint(viewOptions(), index);
//       if (shr.width() > vr.width())
//         return true;
         QToolTip::showText(helpEvent->globalPos(), this->toolTip());
//       else
//         return false;
   }
//   else
       return QAbstractItemView::viewportEvent(event);
}
