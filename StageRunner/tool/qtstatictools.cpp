#include "qtstatictools.h"

#include <QKeySequence>
#include <QDebug>
#include <QRegExp>

QtStaticTools::QtStaticTools()
{
}

int QtStaticTools::stringToKey(const QString &str)
{
	QKeySequence seq(str);
	uint keyCode = 0;

	if(seq.count() >= 1) {
		keyCode = seq[0];
	}

	return keyCode;
}

QString QtStaticTools::keyToString(int key1, int modifier)
{
	QKeySequence seq(key1 + modifier);

	return seq.toString();
}

int QtStaticTools::timeStringToMS(const QString &str)
{
	int time_ms = 0;
	QString number = str.simplified().toLower();
	QString arg = number;
	number.replace(QRegExp("\\D"),"");
	if (arg.endsWith("s")  && !arg.contains("m")) {
		time_ms = number.toInt() * 1000;
	} else {
		time_ms = number.toInt();
	}

	return time_ms;
}

QString QtStaticTools::msToTimeString(int ms)
{
	QString time;
	if (ms >= 10000 || ms%1000 == 0) {
		time = QString("%1s").arg(ms / 1000);
	} else {
		time = QString("%1ms").arg(ms);
	}
	return time;
}
