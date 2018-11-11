#include "qtstatictools.h"

#include <QKeySequence>
#include <QDebug>
#include <QRegExp>
#include <QStringList>

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
	if (arg.startsWith("-"))
		time_ms = -time_ms;

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

QString QtStaticTools::msToTimeString(qint64 ms)
{
	QString time;
	if (ms >= 10000 || ms%1000 == 0) {
		time = QString("%1s").arg(ms / 1000);
	} else {
		time = QString("%1ms").arg(ms);
	}
	return time;
}

QString QtStaticTools::qRectToString(const QRect rect)
{
	QString rs = QString("%1:%2:%3:%4")
			.arg(QString::number(rect.x())
				 ,QString::number(rect.y())
				 ,QString::number(rect.width())
				 ,QString::number(rect.height()));
	return rs;
}

QRect QtStaticTools::stringToQRect(const QString &rs)
{
	QRect rect;
	QStringList parts = rs.split(QChar(':'));

	if (parts.size() == 4) {
		rect.setY(parts[1].toInt());
		rect.setX(parts[0].toInt());
		rect.setHeight(parts[3].toInt());
		rect.setWidth(parts[2].toInt());
	}

	return rect;
}

/**
 * @brief Split string into parts seperated by space char, but not if space is escaped by quotation or backslash
 * @param parastr
 * @return
 */
QStringList QtStaticTools::parameterStringSplit(const QString &parastr)
{
	QStringList finalparas;
	QString para;

	bool quote = false;		// is true, if quotation is active
	QChar last_c;			// stores character parsed in last loop
	for (int i=0; i<parastr.size(); i++) {
		QChar c = parastr.at(i);
		if (c == '\\') {	// found escape character
			if (last_c == c) { // escaped escape char :-) -> add this
				para += c;
			}
		}
		else if (c == '"') {
			if (last_c == '\\') {		// escaped quote
				para += c;
			} else {
				quote = !quote;
			}
		}
		else if (c == ' ') {					// found parameter seperator
			if (quote || last_c == '\\') {		// if quote is active or last char was escape, we add this
				para += c;
			} else {
				finalparas << para;
				para.clear();
			}
		} else {
			para += c;
		}

		last_c = c;
	}

	if (!para.isEmpty() && !quote)
		finalparas << para;

	return finalparas;
}
