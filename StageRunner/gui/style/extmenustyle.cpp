#include "extmenustyle.h"

#include <QStyleFactory>
ExtMenuStyle::ExtMenuStyle()
	: QProxyStyle(QStyleFactory::create("fusion"))
{
}

int ExtMenuStyle::pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
{
	switch (metric) {
	case QStyle::PM_SmallIconSize:
		return 20;
	default:
		return QProxyStyle::pixelMetric(metric, option, widget);
	}
}
