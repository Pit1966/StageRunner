#ifndef QTSTATICTOOLS_H
#define QTSTATICTOOLS_H

#include <QtGlobal>

class QtStaticTools
{
public:
	QtStaticTools();

	static uint stringToKey(const QString & str);
	static QString keyToString(uint key1, uint key2 = 0);

};

#endif // QTSTATICTOOLS_H
