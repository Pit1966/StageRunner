#ifndef LIGHTDESKSTYLE_H
#define LIGHTDESKSTYLE_H

#ifdef IS_QT5
#include <QtWidgets>
#else
#include <QProxyStyle>
#include <QStyleFactory>
#include <QPen>
#include <QPixmap>
#include <QDebug>
#include <QTabWidget>
#include <QPushButton>
#include <QPainter>
#include <QStyleOption>
#include <QGroupBox>
#endif

class LightDeskStyle : public QProxyStyle
{
	Q_OBJECT
protected:
	static int roundRadius;
	static QPen penBevelDarkTrans;
	static QPen penBevelLightTrans;

	QPixmap pixButtonL;
	QPixmap pixButtonM;
	QPixmap pixButtonR;

public:
	LightDeskStyle(const QString &basekey = "windows");
	~LightDeskStyle();

	void polish(QPalette &pal);
	// void polish(QWidget *widget);

	int styleHint(StyleHint hint, const QStyleOption *option, const QWidget *widget, QStyleHintReturn *returnData) const;
	int pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const;
	void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const;
	void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const;
	void drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const;
	void drawItemText(QPainter *painter, const QRect &rect, int flags, const QPalette &pal, bool enabled, const QString &text, QPalette::ColorRole textRole) const;


protected:
	static void setTexture(QPalette &pal, QPalette::ColorRole role, const QPixmap &pixmap);
	static QPainterPath drawRoundBox(const QRect &rect, bool tabFrame = false, int textWidth = 0);
	static QPainterPath drawTabRoundBox(const QRect &rect, QStyle::State state);

signals:

public slots:

};

#endif // LIGHTDESKSTYLE_H
