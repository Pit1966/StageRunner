#ifndef EXTMENUSTYLE_H
#define EXTMENUSTYLE_H

#include <QProxyStyle>

class ExtMenuStyle : public QProxyStyle
{
	Q_OBJECT
public:
	ExtMenuStyle();
	int pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const override;

};

#endif // EXTMENUSTYLE_H
