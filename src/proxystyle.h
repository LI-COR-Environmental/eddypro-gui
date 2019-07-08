/***************************************************************************
  proxystyle.h
  ------------
  Copyright © 2013-2019, LI-COR Biosciences, Inc. All Rights Reserved.
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

#ifndef PROXYSTYLE_H
#define PROXYSTYLE_H

#include <QStyle>

class ProxyStyle : public QStyle
{
public:
    explicit ProxyStyle(const QString& style_);
    ~ProxyStyle() override;

    void drawComplexControl(ComplexControl control, const QStyleOptionComplex* option, QPainter* painter, const QWidget* widget = nullptr) const override;
    void drawControl(ControlElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget = nullptr)  const override;
    void drawItemPixmap(QPainter* painter, const QRect& rect, int alignment, const QPixmap& pixmap) const override;
    void drawItemText(QPainter* painter, const QRect& rect, int alignment, const QPalette& pal, bool enabled, const QString& text, QPalette::ColorRole textRole = QPalette::NoRole) const override;
    void drawPrimitive(PrimitiveElement elem, const QStyleOption* option, QPainter* painter, const QWidget* widget = nullptr) const override;
    QPixmap generatedIconPixmap(QIcon::Mode iconMode, const QPixmap& pixmap, const QStyleOption* option) const override;
    SubControl hitTestComplexControl(ComplexControl control, const QStyleOptionComplex* option, const QPoint& pos, const QWidget* widget = nullptr) const override;
    QRect itemPixmapRect(const QRect& rect, int alignment, const QPixmap& pixmap) const override;
    QRect itemTextRect(const QFontMetrics& metrics, const QRect& rect, int alignment, bool enabled, const QString& text) const override;
    int	layoutSpacing(QSizePolicy::ControlType control1, QSizePolicy::ControlType control2, Qt::Orientation orientation, const QStyleOption * option = nullptr, const QWidget * widget = nullptr) const override;
    int pixelMetric(PixelMetric metric, const QStyleOption* option = nullptr, const QWidget* widget = nullptr) const override;
    void polish(QWidget* widget) override;
    void polish(QApplication* app) override;
    void polish(QPalette& pal) override;
    QSize sizeFromContents(ContentsType type, const QStyleOption* option, const QSize& contentsSize, const QWidget* widget = nullptr) const override;
    QIcon standardIcon(StandardPixmap standardIcon, const QStyleOption* option = nullptr, const QWidget* widget = nullptr) const override;
    QPalette standardPalette() const override;
    QPixmap standardPixmap(StandardPixmap standardPixmap, const QStyleOption* option = nullptr, const QWidget* widget = nullptr) const override;
    int styleHint(StyleHint hint, const QStyleOption* option = nullptr, const QWidget* widget = nullptr, QStyleHintReturn* returnData = nullptr) const override;
    QRect subControlRect(ComplexControl control, const QStyleOptionComplex* option, SubControl subControl, const QWidget* widget = nullptr) const override;
    QRect subElementRect(SubElement element, const QStyleOption* option, const QWidget* widget = nullptr) const override;
    void unpolish(QWidget* widget) override;
    void unpolish(QApplication* app) override;

private:
    QStyle* style_;
};

#endif // PROXYSTYLE_H
