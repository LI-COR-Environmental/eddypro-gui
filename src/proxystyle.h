/***************************************************************************
  proxystyle.h
  -------------------
  Copyright (C) 2013-2014, LI-COR Biosciences
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

#ifndef PROXYSTYLE_H
#define PROXYSTYLE_H

#include <QStyle>

class ProxyStyle : public QStyle
{
public:
	explicit ProxyStyle(const QString& style);
	~ProxyStyle();

    void drawComplexControl(ComplexControl control, const QStyleOptionComplex* option, QPainter* painter, const QWidget* widget = nullptr) const;
    void drawControl(ControlElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget = nullptr)  const;
	void drawItemPixmap(QPainter* painter, const QRect& rect, int alignment, const QPixmap& pixmap) const;
	void drawItemText(QPainter* painter, const QRect& rect, int alignment, const QPalette& pal, bool enabled, const QString& text, QPalette::ColorRole textRole = QPalette::NoRole) const;
    void drawPrimitive(PrimitiveElement elem, const QStyleOption* option, QPainter* painter, const QWidget* widget = nullptr) const;
	QPixmap generatedIconPixmap(QIcon::Mode iconMode, const QPixmap& pixmap, const QStyleOption* option) const;
    SubControl hitTestComplexControl(ComplexControl control, const QStyleOptionComplex* option, const QPoint& pos, const QWidget* widget = nullptr) const;
	QRect itemPixmapRect(const QRect& rect, int alignment, const QPixmap& pixmap) const;
	QRect itemTextRect(const QFontMetrics& metrics, const QRect& rect, int alignment, bool enabled, const QString& text) const;
    int	layoutSpacing(QSizePolicy::ControlType control1, QSizePolicy::ControlType control2, Qt::Orientation orientation, const QStyleOption * option = nullptr, const QWidget * widget = nullptr) const;
    int pixelMetric(PixelMetric metric, const QStyleOption* option = nullptr, const QWidget* widget = nullptr) const;
    void polish(QWidget* widget);
	void polish(QApplication* app);
	void polish(QPalette& pal);
    QSize sizeFromContents(ContentsType type, const QStyleOption* option, const QSize& contentsSize, const QWidget* widget = nullptr) const;
    QIcon standardIcon(StandardPixmap standardIcon, const QStyleOption* option = nullptr, const QWidget* widget = nullptr) const;
	QPalette standardPalette() const;
    QPixmap standardPixmap(StandardPixmap standardPixmap, const QStyleOption* option = nullptr, const QWidget* widget = nullptr) const;
    int styleHint(StyleHint hint, const QStyleOption* option = nullptr, const QWidget* widget = nullptr, QStyleHintReturn* returnData = nullptr) const;
    QRect subControlRect(ComplexControl control, const QStyleOptionComplex* option, SubControl subControl, const QWidget* widget = nullptr) const;
    QRect subElementRect(SubElement element, const QStyleOption* option, const QWidget* widget = nullptr) const;
	void unpolish(QWidget* widget);
	void unpolish(QApplication* app);

private:
	QStyle* style;
};

#endif // PROXYSTYLE_H
