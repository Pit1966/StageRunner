#include "qtstatictools.h"

#include <QKeySequence>
#include <QDebug>

QtStaticTools::QtStaticTools()
{
}

uint QtStaticTools::stringToKey(const QString &str)
{
	QKeySequence seq(str);
	uint keyCode = 0;

	if(seq.count() >= 1) {
		keyCode = seq[0];
	}

	return keyCode;
}

QString QtStaticTools::keyToString(uint key1)
{
	QKeySequence seq(key1);

	return seq.toString();
}
