/***************************************************************************
  mystyle.cpp
  ---------
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
#include "mystyle.h"

int MyStyle::pixelMetric(QStyle::PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
{
    // TODO: to verify
    // hack for the menu icons size
#if defined(Q_OS_WIN)
    if (metric == QStyle::PM_SmallIconSize)
        return 16;
#endif

//    if (metric == PM_SplitterWidth)
//        return 10;

    // TODO: general, customize
//    if (metric == PM_DockWidgetSeparatorExtent)
//        return 7;

//    if (metric == PM_DockWidgetHandleExtent)
//        return 7;

    if (metric == PM_DockWidgetTitleBarButtonMargin)
        return 0;

    if (metric == PM_DockWidgetTitleMargin)
        return 0;

    return ProxyStyle::pixelMetric(metric, option, widget);
}
