#include "variantmapserializer.h"

#include <QDebug>

VariantMapSerializer::VariantMapSerializer()
{

}

QString VariantMapSerializer::toString(const QVariantMap map)
{
	QString out;
	out += "{";
	int elements = 0;
	for (const QString &key : map.keys()) {
		if (elements > 0)
			out += ",";		// add seperator for next element

		const QVariant &var = map.value(key);
		out += '"';
		out += key;
		out += '"';
		out += ":";
		switch (var.type()) {
		case QVariant::String:
			out += '"';
			out += var.toString();
			out += '"';
			break;

		case QVariant::Int:
		case QVariant::UInt:
			out += var.toString();
			break;

		default:
			qDebug() << __func__ << "Format not supported:" << var.typeName();
		}

		elements++;
	}

	out += "}";
	return out;
}

QVariantMap VariantMapSerializer::toMap(const QString &str)
{
	QVariantMap outmap;
	int position = 0;
	bool ok = parseString(position, str, outmap);
	if (!ok)
		qDebug() << __func__ << "parse error in:" << str;

	return outmap;
}

bool VariantMapSerializer::parseString(int &pos, const QString &str, QVariantMap &map)
{
	int size = str.size();
	char c = getNextSkipSpace(pos, str);

	switch (c) {
	case '{':
		// get next character
		c = getNextSkipSpace(pos, str);
		if (c == '}') { // object is empty
			return true;
		}

		while (1) {
			if (c == '"') { // this should introduce the beginning key / data pair
				// get key string
				QString curkey;
				if (!getString(pos, str, curkey))
					return false;

				// look for ":"
				c = getNextSkipSpace(pos,str);
				if (c != ':')
					return false;

				// now a data value should follow
				c = getNextSkipSpace(pos,str);
				if (c == '"') { // ok, this is a string type
					QString val;
					if (!getString(pos,str,val))
						return false;
					// add key / string pair to map
					map.insert(curkey, val);
				}
				else if ( (c >= '0' and c <= '9') || c == '-' || c == '+' || c == '.' ) { // a number
					// rollback position, cause we have already consumed the first number character
					pos--;
					double number;
					getNumber(pos,str,number);
					// add key / number pair to map, where number maybe double or long long
					if (number - int(number) == 0) {
						map.insert(curkey,int(number));
					} else {
						map.insert(curkey,number);
					}
				}
				else if (c == '[') {
					qDebug() << __func__ << "Array not supported yet";
					return false;
				}
				else if (c == '{') {
					qDebug() << __func__ << "Object inside object not supported yet";
					return false;
				}
				else {
					return false;
				}

				// A key/data value is fetched now. Lets see if there are more
				c = getNextSkipSpace(pos,str);
				if (c == '}') {
					// object end, leave the loop
					break;
				}
				else if (c == ',') {
					// get the next character. Hopefully a '"'
					c = getNextSkipSpace(pos,str);
				}
			} else {
				return false;
			}
		}

		break;

	default:
		return false;
	}

	return true;
}

bool VariantMapSerializer::getString(int &pos, const QString &str, QString &target)
{
	QChar c;
	while (pos<str.size()) {
		c = str.at(pos);
		pos++;
		if (c == '"')
			break;
		target += c;
	}

	if (c != '"')
		return false;

	return true;
}

bool VariantMapSerializer::getNumString(int &pos, const QString &str, QString &target)
{
	char c;
	while (pos<str.size()) {
		c = str.at(pos).toLatin1();
		switch (c) {
		case ',':
		case '}':
		case ']':
		case ' ':
			return true;

		default:
			target += c;
			pos++;
		}
	}

	return false;
}

bool VariantMapSerializer::getNumber(int &pos, const QString &str, double &number)
{
	QString numStr;
	if (!getNumString(pos,str,numStr))
		return false;

	bool ok;
	number = numStr.toDouble(&ok);
	return ok;
}

char  VariantMapSerializer::getNextSkipSpace(int &pos, const QString &str)
{
	char c = 0;
	while (pos<str.size()) {
		c = str.at(pos).toLatin1();
		pos++;
		if (c != ' ')
			break;
	}

	return c;
}
