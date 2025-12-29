/***********************************************************************************
//  StageRunner
//  Multi platform software that controls sound effects, sound recordings
//  and lighting systems on small to medium-sized stages
//
//  Copyright (C) 2013-2026 by Peter Steinmeyer (Pit1966 on github)
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
************************************************************************************/

#ifndef QTSTATICTOOLS_H
#define QTSTATICTOOLS_H

#include <QtGlobal>
#include <QRect>
#include <QStringList>

class QtStaticTools
{
public:
	QtStaticTools();

	static int stringToKey(const QString & str);
	static QString keyToString(int key1, int modifier = 0);
	static int timeStringToMS(const QString &str);
	static QString msToTimeString(int ms);
	static QString msToTimeString(qint64 ms);
	static QString qRectToString(const QRect rect);
	static QRect stringToQRect(const QString &rs);
	static QStringList parameterStringSplit(const QString &parastr);

	static bool checkCreateDir(const QString &path, bool *dirAlreadyExists);
};

#endif // QTSTATICTOOLS_H
