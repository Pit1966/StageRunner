#ifndef VARIANTMAPSERIALIZER_H
#define VARIANTMAPSERIALIZER_H

#include <QVariant>
#include <QMap>
#include <QVariantMap>

class VariantMapSerializer
{
public:
	VariantMapSerializer();

	static QString toString(const QVariantMap map);
	static QVariantMap toMap(const QString &str);
private:
	static bool parseString(int &pos, const QString &str, QVariantMap &map);
	static bool getString(int &pos, const QString &str, QString &target);
	static bool getNumString(int &pos, const QString &str, QString &target);
	static bool getNumber(int &pos, const QString &str, double &number);
	static char getNextSkipSpace(int &pos, const QString &str);
};

#endif // VARIANTMAPSERIALIZER_H
