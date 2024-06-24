//=======================================================================
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2019 by Peter Steinmeyer (Pit1966 on github)
//  (C) Copyright 2019 stonechip entertainment
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; version 2 of
//  the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//=======================================================================

#ifndef VARIANTMAPSERIALIZER_H
#define VARIANTMAPSERIALIZER_H

#include <QVariant>
#include <QMap>
#include <QVariantMap>

class VariantMapSerializer
{
public:
	VariantMapSerializer();

	static QString toString(const QVariantMap &map);
	static QVariantMap toMap(const QString &str);
private:
	static bool parseString(int &pos, const QString &str, QVariantMap &map);
	static bool getString(int &pos, const QString &str, QString &target);
	static bool getNumString(int &pos, const QString &str, QString &target);
	static bool getNumber(int &pos, const QString &str, double &number);
	static char getNextSkipSpace(int &pos, const QString &str);
};

#endif // VARIANTMAPSERIALIZER_H
