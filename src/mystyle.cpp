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
