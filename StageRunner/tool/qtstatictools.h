#ifndef QTSTATICTOOLS_H
#define QTSTATICTOOLS_H

#include <QtGlobal>
#include <QRect>

class QtStaticTools
{
public:
	QtStaticTools();

	static int stringToKey(const QString & str);
	static QString keyToString(int key1, int modifier = 0);
	static int timeStringToMS(const QString &str);
	static QString msToTimeString(int ms);
	static QString qRectToString(const QRect rect);
	static QRect stringToQRect(const QString &rs);

};

#endif // QTSTATICTOOLS_H
