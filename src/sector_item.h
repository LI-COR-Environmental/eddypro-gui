#ifndef SECTORITEM_H
#define SECTORITEM_H

#include <QColor>

struct SectorItem
{
    SectorItem(double startAngle = 0.0,
               double endAngle = 10.0,
               QColor color = QColor(170, 230, 255)) // #aae6ff
        : startAngle_(startAngle),
          endAngle_(endAngle),
          color_(std::move(color))
    {
    }

    double startAngle_;
    double endAngle_;
    QColor color_;
};

#endif // SECTORITEM_H
