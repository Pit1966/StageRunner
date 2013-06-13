#ifndef QTSTATICTOOLS_H
#define QTSTATICTOOLS_H

#include <QtGlobal>

class QtStaticTools
{
public:
	QtStaticTools();

	static int stringToKey(const QString & str);
	static QString keyToString(int key1, int modifier = 0);
	static int timeStringToMS(const QString &str);
	static QString msToTimeString(int ms);

};

#endif // QTSTATICTOOLS_H
